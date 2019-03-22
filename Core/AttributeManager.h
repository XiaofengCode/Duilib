#pragma once
#include <map>
#include <vector>
#include <queue>
#include <list>
namespace DuiLib{	

class CDuiImageItem
{
public:
	CDuiImageItem()
	{
		SetEmpty();
	}
	void SetEmpty()
	{
		m_byFade = 255;
		ZeroMemory(&m_rcSrc, sizeof(m_rcSrc));
		ZeroMemory(&m_rcDst, sizeof(m_rcDst));
		ZeroMemory(&m_rcCorner, sizeof(m_rcCorner));
		m_dwMask = 0;
		m_bHole = false;
		m_bXTiled = false;
		m_bYTiled = false;
		m_bRight = false;
		m_bBottom = false;
		m_strFile = _T("");
		m_strImageResType = _T("");
	}
	//(bk.bmp或file='aaa.jpg' res='' restype='0' dest='0,0,0,0' source='0,0,0,0' corner='0,0,0,0' mask='#FF0000' fade='255' hole='false' xtiled='false' ytiled='false')
	BYTE m_byFade;
	CDuiString m_strFile;
	CDuiString m_strImageResType;
	RECT m_rcSrc;
	RECT m_rcDst;
	RECT m_rcCorner;
	DWORD m_dwMask;
	bool m_bHole;
	bool m_bXTiled;
	bool m_bYTiled;
	bool m_bRight;
	bool m_bBottom;
};
typedef std::vector<CDuiImageItem> CDuiImageItemArray;

class CDuiImage
{
public:
	CDuiImage()
	{
	}
	~CDuiImage()
	{
	}

	void AddItem(const CDuiImageItem& item)
	{
		m_arItem.push_back(item);
	}

	int GetItemCount() const
	{
		return m_arItem.size();
	}

	CDuiImageItem& operator[](int nIndex)
	{
		return m_arItem[nIndex];
	}

	const CDuiImageItem& operator[](int nIndex) const
	{
		return m_arItem[nIndex];
	}
	operator bool() const
	{
		return m_arItem.size() != 0;
	}
protected:
	CDuiImageItemArray m_arItem;
};


enum ValueType{
	TypeUnknown,
	TypeInt,
	TypeColor,
	TypeRect,
	TypeString,
	TypeBool,
	TypePoint,
	TypePercent,
	TypeIntOrPercent,
	TypeIntOrRect,
	TypeIntOrString,
	TypeImage,
	TypeMax,
};


class CAttrItem 
{
public:
	CAttrItem()
	{
		value.pValue = nullptr;
		pRef = new LONG();
		*pRef = 1;
	}
	~CAttrItem()
	{
		Release();
	}
	void Release()
	{
		if (!pRef)
		{
			return;
		}
		if (InterlockedDecrement(pRef))
		{
			value.pValue = nullptr;
			return;
		}
		if (!value.pValue)
		{
			return;
		}
		switch (realtype)
		{
		case TypeInt:
			delete value.pIntValue;
			break;
		case TypeColor:
			delete value.pDwordValue;
			break;
		case TypeRect:
			delete value.pRectValue;
			break;
		case TypeString:
			delete value.pStringValue;
			break;
		case TypeBool:
			delete value.pBoolValue;
			break;
		case TypePoint:
			delete value.pPointValue;
			break;
		case TypePercent:
			delete value.pIntValue;
			break;
		case TypeImage:
			delete value.pImageValue;
			break;
		}
	}
	CAttrItem& operator =(const CAttrItem& src)
	{
		Release();
		InterlockedIncrement(src.pRef);
		pRef = src.pRef;
		type = src.type;
		realtype = src.realtype;
		strValue = src.strValue;
		value.pValue = src.value.pValue;
		return *this;
	}

	ValueType type;
	ValueType realtype;
	CDuiString strValue;
	PLONG	pRef;	//共用下面指针的引用计数
	union
	{
		int* pIntValue;
		DWORD* pDwordValue;
		bool* pBoolValue;
		RECT* pRectValue;
		POINT* pPointValue;
		CDuiString* pStringValue;
		void* pValue;
		CDuiImage* pImageValue;
	}value;
};

const static POINT POINT_NULL = {0};
const static RECT RECT_NULL = {0};
const static CDuiImage DUIIMAGE_NULL;
const static CAttrItem ATTRITEM_NULL;

/*
	首先给每个关键字编号
	然后建立树形结构
	
*/
class CAttrTreeNode;
typedef std::map<DWORD, CAttrTreeNode*> CMapAttrNodes;
typedef std::queue<DWORD> CAttrIDQueue;

class CAttrTreeNode
{
public:
	CAttrTreeNode()
	{
		m_pValue = nullptr;
		m_pMapSubNodes = nullptr;	
	}
	CAttrTreeNode(CAttrIDQueue arSubAttrIDs, const CAttrItem& value)
	{
		m_pValue = nullptr;
		m_pMapSubNodes = nullptr;
		//arAttrIDs.pop();
		SetSubValue(arSubAttrIDs, value);
	}
	const CAttrItem& GetSubValue(CAttrIDQueue arAttrIDs) const
	{
		if (arAttrIDs.size() == 0)
		{
			if (!m_pValue)
			{
				return ATTRITEM_NULL;
			}
			return *m_pValue;
		}
		if (!m_pMapSubNodes)
		{
			return ATTRITEM_NULL;
		}

		CMapAttrNodes::iterator itor = m_pMapSubNodes->find(arAttrIDs.front());
		arAttrIDs.pop();
		if (itor == m_pMapSubNodes->end())
		{
			return ATTRITEM_NULL;
		}
		return itor->second->GetSubValue(arAttrIDs);
	}
	void SetSubValue(CAttrIDQueue arAttrIDs, const CAttrItem& value)
	{
		if (arAttrIDs.size() == 0)
		{
			if (!m_pValue)
			{
				m_pValue = new CAttrItem();
			}
			*m_pValue = value;
			return;
		}
		if (!m_pMapSubNodes)
		{
			m_pMapSubNodes = new CMapAttrNodes();
		}

		DWORD dwSubID = arAttrIDs.front();
		arAttrIDs.pop();
		CMapAttrNodes::iterator itor = m_pMapSubNodes->find(dwSubID);
		if (itor == m_pMapSubNodes->end())
		{
			(*m_pMapSubNodes)[dwSubID] = new CAttrTreeNode(arAttrIDs, value);
		}
		else
		{
			//ASSERT(itor != m_pMapSubNodes->end());
			itor->second->SetSubValue(arAttrIDs, value);
		}
	}
protected:
	
	CAttrItem* m_pValue;
	CMapAttrNodes* m_pMapSubNodes;
};

class CAttributeManager
{
public:
	typedef std::map<CDuiString, CAttrItem> CMapAttrs;
	//typedef std::map<CDuiString, DWORD> CMapStatus;
	typedef std::pair<CDuiString, DWORD> CPairKeyword;
	typedef std::list<CPairKeyword> CListKeywords;
	typedef bool (*PFN_ParseAttrValue)(LPCTSTR lpszValue, CAttrItem& item);
	CAttributeManager();
	virtual ~CAttributeManager()
	{
	}
	void SetMetaManager(const CAttributeManager* pMgr);
	void AddKeyword(LPCTSTR lpszAttrKeyword, ValueType type = TypeUnknown);

	bool SetAttribute(LPCTSTR lpszAttr, LPCTSTR lpszValue);
	bool SetAttribute(LPCTSTR lpszAttr, int value);
	bool SetAttribute(LPCTSTR lpszAttr, DWORD value, ValueType type = TypeColor);
	bool SetAttribute(LPCTSTR lpszAttr, RECT value);
	bool SetAttribute(LPCTSTR lpszAttr, bool value);
	bool SetAttribute(LPCTSTR lpszAttr, POINT value);

	CDuiString GetString(LPCTSTR lpszAttr, bool* pResult = NULL) const
	{
		CDuiString strAttr(lpszAttr);
		strAttr.MakeLower();
		CAttrItem item;
		bool bRet = GetAttributeItem(strAttr, item);
		if (pResult)
		{
			*pResult = bRet;
		}
		return item.strValue;
	}
#define GETATTRIBUTE(retType, name, valueType, retValue, invalidValue)\
	retType name(LPCTSTR lpszAttr, bool* pResult = NULL) const\
	{\
	CDuiString strAttr(lpszAttr);\
	strAttr.MakeLower();\
	CAttrItem item;\
	bool bRet = GetAttributeItem(strAttr, item);\
	if (pResult)\
	{\
	*pResult = bRet;\
	}\
	if (!bRet)\
	{\
	return invalidValue;\
	}\
	if (item.realtype != valueType && valueType != TypeString)\
	{\
	if (pResult)\
	{\
	*pResult = false;\
	}\
	}\
		else\
	{\
	return retValue;\
	}\
	return invalidValue;\
	}
	GETATTRIBUTE(int, GetInt, TypeInt, *item.value.pIntValue, 0)
	GETATTRIBUTE(DWORD, GetColor, TypeColor, *item.value.pDwordValue, 0)
	GETATTRIBUTE(RECT, GetRect, TypeRect, *item.value.pRectValue, RECT_NULL)
	//GETATTRIBUTE(LPCTSTR, GetString, TypeString, item.value.pStringValue, _T(""))
	GETATTRIBUTE(bool, GetBool, TypeBool, *item.value.pBoolValue, false)
	GETATTRIBUTE(POINT, GetPoint, TypePoint, *item.value.pPointValue, POINT_NULL)
	GETATTRIBUTE(int, GetPercent, TypePercent, *item.value.pIntValue, 0)
	GETATTRIBUTE(CDuiImage, GetImage, TypeImage, *item.value.pImageValue, DUIIMAGE_NULL)
protected:
	bool ParseAttributeValue(LPCTSTR lpszAttr, LPCTSTR lpszValue);
	ValueType ParseStatus(LPCTSTR lpszAttr, CAttrIDQueue& queAttr) const;
	bool GetAttributeItem(LPCTSTR lpszAttr, CAttrItem& item) const;
	static bool ParseInt(LPCTSTR lpszValue, CAttrItem& item);
	static bool ParseColor(LPCTSTR lpszValue, CAttrItem& item);
	static bool ParseRect(LPCTSTR lpszValue, CAttrItem& item);
	static bool ParseString(LPCTSTR lpszValue, CAttrItem& item);
	static bool ParseBool(LPCTSTR lpszValue, CAttrItem& item);
	static bool ParsePoint(LPCTSTR lpszValue, CAttrItem& item);
	static bool ParsePercent(LPCTSTR lpszValue, CAttrItem& item);
	static bool ParseIntOrPercent(LPCTSTR lpszValue, CAttrItem& item);
	static bool ParseIntOrRect(LPCTSTR lpszValue, CAttrItem& item);
	static bool ParseIntOrString(LPCTSTR lpszValue, CAttrItem& item);
	static bool ParseImage(LPCTSTR lpszValue, CAttrItem& item);
	const static PFN_ParseAttrValue pfnParseFunctions[TypeMax];
 	//CMapStatus m_mapKeywords;
	CListKeywords m_lstKeywords;
// 	CMapAttrs m_mapAttrs;
	CAttrTreeNode	m_AttrTreeRoot;
	const CAttributeManager* m_pMetaManager;
};

}