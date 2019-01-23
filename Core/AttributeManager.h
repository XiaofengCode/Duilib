#pragma once
namespace DuiLib{
const static POINT POINT_NULL = {0};
const static RECT RECT_NULL = {0};
class CAttributeManager
{
public:
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
		TypeMax,
	};
	struct ATTR_ITEM 
	{
		ValueType type;
		ValueType realtype;
		CDuiString strValue;
		union
		{
			int* pIntValue;
			DWORD* pDwordValue;
			bool* pBoolValue;
			RECT* pRectValue;
			POINT* pPointValue;
			CDuiString* pStringValue;
			void* pValue;
		}value;
	};
	typedef std::map<CDuiString, ATTR_ITEM> CMapAttrs;
	typedef std::map<CDuiString, DWORD> CMapStatus;
	typedef bool (*PFN_ParseAttrValue)(LPCTSTR lpszValue, ATTR_ITEM& item);
	virtual ~CAttributeManager()
	{
	}
	void AddBindStatus(LPCTSTR lpszAttrPrefix, DWORD dwStatus);
	//bool AddBindAttribute(LPCTSTR lpszAttr, ValueType type, LPCTSTR lpszDefaultValue);
// 	bool AddBindAttribute(LPCTSTR lpszAttr, ValueType type, int* pBind);
// 	bool AddBindAttribute(LPCTSTR lpszAttr, ValueType type, DWORD* pBind);
// 	bool AddBindAttribute(LPCTSTR lpszAttr, ValueType type, RECT* pBind);
// 	bool AddBindAttribute(LPCTSTR lpszAttr, ValueType type, bool* pBind);
// 	bool AddBindAttribute(LPCTSTR lpszAttr, ValueType type, POINT* pBind);
	bool AddBindAttribute(LPCTSTR lpszAttr, ValueType type, void* pBind);

	bool SetAttribute(LPCTSTR lpszAttr, LPCTSTR lpszValue);
#define GETATTRIBUTE(retType, name, valueType, retValue, invalidValue)\
	retType name(LPCTSTR lpszAttr, DWORD dwStatus = 0, bool* pResult = NULL)\
	{\
	CDuiString strAttr(lpszAttr);\
	strAttr.MakeLower();\
	ATTR_ITEM item;\
	bool bRet = GetAttributeItem(strAttr, item, dwStatus);\
	if (pResult)\
	{\
	*pResult = bRet;\
	}\
	if (!bRet)\
	{\
	return invalidValue;\
	}\
	if (item.type != valueType && valueType != TypeString)\
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
	GETATTRIBUTE(LPCTSTR, GetString, TypeString, *item.value.pStringValue, _T(""))
	GETATTRIBUTE(bool, GetBool, TypeBool, *item.value.pBoolValue, false)
	GETATTRIBUTE(POINT, GetPoint, TypePoint, *item.value.pPointValue, POINT_NULL)
	GETATTRIBUTE(int, GetPercent, TypePercent, *item.value.pIntValue, 0)
protected:
	bool ParseAttributeValue(LPCTSTR lpszAttr, LPCTSTR lpszValue, DWORD dwStatus = 0);
	LPCTSTR ParseStatus(LPCTSTR lpszAttr, DWORD& dwStatus);
	bool GetAttributeItem(LPCTSTR lpszAttr, ATTR_ITEM& item, DWORD dwStatus = 0);
	static bool ParseInt(LPCTSTR lpszValue, ATTR_ITEM& item);
	static bool ParseColor(LPCTSTR lpszValue, ATTR_ITEM& item);
	static bool ParseRect(LPCTSTR lpszValue, ATTR_ITEM& item);
	static bool ParseString(LPCTSTR lpszValue, ATTR_ITEM& item);
	static bool ParseBool(LPCTSTR lpszValue, ATTR_ITEM& item);
	static bool ParsePoint(LPCTSTR lpszValue, ATTR_ITEM& item);
	static bool ParsePercent(LPCTSTR lpszValue, ATTR_ITEM& item);
	static bool ParseIntOrPercent(LPCTSTR lpszValue, ATTR_ITEM& item);
	static bool ParseIntOrRect(LPCTSTR lpszValue, ATTR_ITEM& item);
	static bool ParseIntOrString(LPCTSTR lpszValue, ATTR_ITEM& item);
	const static PFN_ParseAttrValue pfnParseFunctions[TypeMax];
	CMapStatus m_mapStatuses;
	CMapAttrs m_mapAttrs;
};
}