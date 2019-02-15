#include "duipub.h"
#include "AttributeManager.h"

namespace DuiLib{
	
/*
采用数字表示属性
	状态相关：
		focused		0x00000001
		selected	0x00000002
		disabled	0x00000004
		hot			0x00000008
		pushed		0x00000010
		readonly	0x00000020
		captured	0x00000040

	位置相关：
		back/bk/bg	0x00000001
		fore		0x00000002
		normal		0x00000004
		border		0x00000008
		left		0x00000010
		top			0x00000020
		right		0x00000040
		bottom		0x00000080
		text		0x00000100
	
	附加：
		min
		max
		relative

		可修饰属性：
		color
		pos/position
		padding
		colorhsl
		size
		style
		round
		width
		height
		font

	独立属性：
		tooltip
		userdata
		enabled
		keyboard
		visible
		float
		shortcut
		menu
		virtualwnd
		style

	name
*/
const CAttributeManager::PFN_ParseAttrValue CAttributeManager::pfnParseFunctions[TypeMax] = {
	NULL,
	CAttributeManager::ParseInt,
	CAttributeManager::ParseColor,
	CAttributeManager::ParseRect,
	CAttributeManager::ParseString,
	CAttributeManager::ParseBool,
	CAttributeManager::ParsePoint,
	CAttributeManager::ParsePercent,
	CAttributeManager::ParseIntOrPercent,
	CAttributeManager::ParseIntOrRect,
	CAttributeManager::ParseIntOrString,
	CAttributeManager::ParseImage
};

void CAttributeManager::AddKeyword(LPCTSTR lpszAttrKeyword, ValueType type)
{
	if (m_mapKeywords.find(lpszAttrKeyword) == m_mapKeywords.end())
	{
		m_mapKeywords[lpszAttrKeyword] = (DWORD)MAKELONG(m_mapKeywords.size(), type);
	}
}


bool CAttributeManager::SetAttribute(LPCTSTR lpszAttr, LPCTSTR lpszValue)
{
	CDuiString strAttr(lpszAttr);
	strAttr.MakeLower();
	return ParseAttributeValue(strAttr, lpszValue);
}

bool CAttributeManager::SetAttribute(LPCTSTR lpszAttr, int value)
{
	CAttrIDQueue attrIds;
	ValueType type = ParseStatus(lpszAttr, attrIds);
	if (type == TypeUnknown)
	{
		return false;
	}
	CAttrItem item;
	item.type = type;
	item.realtype = TypeInt;
	item.strValue.SmallFormat(_T("%d"), value);
	if (!item.value.pIntValue)
	{
		item.value.pIntValue = new int;
	}
	*item.value.pIntValue = value;
	m_AttrTreeRoot.SetSubValue(attrIds, item);
	return true;
}

bool CAttributeManager::SetAttribute(LPCTSTR lpszAttr, DWORD value, ValueType type /*= TypeColor*/)
{
	CAttrIDQueue attrIds;
	ValueType typeOld = ParseStatus(lpszAttr, attrIds);
	if (typeOld == TypeUnknown)
	{
		return false;
	}
	CAttrItem item;
	item.type = typeOld;
	item.realtype = type;
	if (type == TypeColor)
	{
		item.strValue.SmallFormat(_T("#%08X"), value);
	}
	else
	{
		item.strValue.SmallFormat(_T("%u"), value);
	}
	if (!item.value.pDwordValue)
	{
		item.value.pDwordValue = new DWORD;
	}
	*item.value.pDwordValue = value;
	m_AttrTreeRoot.SetSubValue(attrIds, item);
	return true;
}

bool CAttributeManager::ParseAttributeValue(LPCTSTR lpszAttr, LPCTSTR lpszValue)
{
	CAttrIDQueue attrIds;
	ValueType type = ParseStatus(lpszAttr, attrIds);
	if (type == TypeUnknown)
	{
		return false;
	}
	CAttrItem value;
	value.type = type;
	value.realtype = TypeUnknown;
	value.strValue = lpszValue;
	if (!pfnParseFunctions[type](lpszValue, value))
	{
		return false;
	}
	m_AttrTreeRoot.SetSubValue(attrIds, value);
	return true;
}

ValueType CAttributeManager::ParseStatus(LPCTSTR lpszAttr, CAttrIDQueue& queAttr) const
{
	std::vector<DWORD> dwAttrIDs;
	CMapStatus::const_iterator itor = m_mapKeywords.find(lpszAttr);
	if (itor == m_mapKeywords.end())
	{
		//没找到就找前缀
		//CDuiString strPrefix(lpszAttr);
		while (*lpszAttr)
		{
			for (CMapStatus::const_iterator itor2 = m_mapKeywords.begin(); itor2 != m_mapKeywords.end(); itor2++)
			{
				if (_tcsnicmp(lpszAttr, itor2->first, itor2->first.GetLength()) == 0)
				{
					dwAttrIDs.push_back(itor2->second);
					lpszAttr += itor2->first.GetLength();
					break;
				}
			}
		}
	}
	else
	{
		dwAttrIDs.push_back(itor->second);
	}
	if (!dwAttrIDs.size())
	{
		return TypeUnknown;
	}
	ValueType ret = (ValueType)HIWORD(dwAttrIDs.back());
	std::sort(dwAttrIDs.begin(), dwAttrIDs.end());
	for (size_t i = 0; i < dwAttrIDs.size(); i++)
	{
		queAttr.push(dwAttrIDs[i]);
	}
	return ret;
}

bool CAttributeManager::GetAttributeItem(LPCTSTR lpszAttr, CAttrItem& item) const
{
	CAttrIDQueue attrIds;
	ValueType type = ParseStatus(lpszAttr, attrIds);
	if (type == TypeUnknown)
	{
		return false;
	}
	item = m_AttrTreeRoot.GetSubValue(attrIds);
	return true;
}

bool CAttributeManager::ParseInt(LPCTSTR lpszValue, CAttrItem& item)
{
	LPTSTR p;
	int i = _tcstol(lpszValue, &p, 10);
	if (p == lpszValue)
	{
		return false;
	}
	item.realtype = TypeInt;
	if (!item.value.pIntValue)
	{
		item.value.pIntValue = new int;
	}
	*item.value.pIntValue = i;
	return true;
}

bool CAttributeManager::ParseColor(LPCTSTR lpszValue, CAttrItem& item)
{
	if( *lpszValue == _T('#')) lpszValue = ::CharNext(lpszValue);
	LPTSTR pstr = NULL;
	DWORD clrColor = _tcstoul(lpszValue, &pstr, 16);
	if (pstr == lpszValue)
	{
		return false;
	}
	item.realtype = TypeColor;
	if (!item.value.pDwordValue)
	{
		item.value.pDwordValue = new DWORD;
	}
	*item.value.pDwordValue = clrColor;
	return true;
}

bool CAttributeManager::ParseRect(LPCTSTR lpszValue, CAttrItem& item)
{
	RECT rc = {0};
	LPTSTR pstr = NULL;
	LPTSTR pstr2 = NULL;
	rc.left = (LONG)(_tcstol(lpszValue, &pstr, 10));
	if (pstr == lpszValue || *pstr == 0)
	{
		return false;
	}
	rc.top = (LONG)(_tcstol(pstr + 1, &pstr2, 10));
	if (pstr == pstr2 || *pstr2 == 0)
	{
		return false;
	}
	rc.right = (LONG)(_tcstol(pstr2 + 1, &pstr, 10));
	if (pstr == pstr2 || *pstr == 0)
	{
		return false;
	}
	rc.bottom = (LONG)(_tcstol(pstr + 1, &pstr2, 10));
	if (pstr == pstr2)
	{
		return false;
	}
	item.realtype = TypeRect;
	if (!item.value.pRectValue)
	{
		item.value.pRectValue = new RECT;
	}
	*item.value.pRectValue = rc;
	return true;
}

bool CAttributeManager::ParseString(LPCTSTR lpszValue, CAttrItem& item)
{
	item.realtype = TypeString;
	item.strValue = lpszValue;
	if (!item.value.pStringValue)
	{
		item.value.pStringValue = new CDuiString();
	}
	*item.value.pStringValue = lpszValue;
	return true;
}

bool CAttributeManager::ParseBool(LPCTSTR lpszValue, CAttrItem& item)
{
	if (_tcsicmp(lpszValue, _T("true")) == 0
		|| _tcsicmp(lpszValue, _T("yes")) == 0)
	{
		item.realtype = TypeBool;
		*item.value.pBoolValue = true;
		return true;
	}
	item.realtype = TypeBool;
	if (!item.value.pBoolValue)
	{
		item.value.pBoolValue = new bool;
	}
	*item.value.pBoolValue = false;
	return true;
}

bool CAttributeManager::ParsePoint(LPCTSTR lpszValue, CAttrItem& item)
{
	POINT pt;
	LPTSTR pstr = NULL;
	LPTSTR pstr2 = NULL;
	pt.x = (LONG)(_tcstol(lpszValue, &pstr, 10));
	if (pstr == lpszValue || *pstr == 0)
	{
		return false;
	}
	pt.y = (LONG)(_tcstol(pstr + 1, &pstr2, 10));
	if (pstr == pstr2)
	{
		return false;
	}
	if (!item.value.pPointValue)
	{
		item.value.pPointValue = new POINT;
	}
	*item.value.pPointValue = pt;
	return true;
}

bool CAttributeManager::ParsePercent(LPCTSTR lpszValue, CAttrItem& item)
{
	LPCTSTR p = _tcschr(lpszValue, '%');
	if (!p)
	{
		return false;
	}
	if (!ParseInt(lpszValue, item))
	{
		return false;
	}
	return true;
}

bool CAttributeManager::ParseIntOrPercent(LPCTSTR lpszValue, CAttrItem& item)
{
	if (ParsePercent(lpszValue, item))
	{
		return true;
	}
	if (ParseInt(lpszValue, item))
	{
		return true;
	}
	return false;
}

bool CAttributeManager::ParseIntOrRect(LPCTSTR lpszValue, CAttrItem& item)
{
	if (ParseRect(lpszValue, item))
	{
		return true;
	}
	if (ParseInt(lpszValue, item))
	{
		return true;
	}
	return false;
}

bool CAttributeManager::ParseIntOrString(LPCTSTR lpszValue, CAttrItem& item)
{
	if (ParseInt(lpszValue, item))
	{
		return true;
	}
	if (ParseString(lpszValue, item))
	{
		return true;
	}
	return false;
}

bool CAttributeManager::ParseImage(LPCTSTR lpszValue, CAttrItem& item)
{
	CDuiString sImageResType;

	CDuiString sItem;
	CDuiString sValue;
	LPTSTR pstr = NULL;

	if( !lpszValue )
		return false;

	CDuiImageItem img;
	CDuiImage* pImage = new CDuiImage();
	CDuiString strValue(lpszValue);
	while( *lpszValue != _T('\0') )
	{
		sItem.Empty();
		sValue.Empty();
		while( *lpszValue > _T('\0') && *lpszValue <= _T(' ') )
		{
			lpszValue = ::CharNext(lpszValue);
		}
		while( *lpszValue != _T('\0') && *lpszValue != _T('=') && *lpszValue > _T(' ') )
		{
			LPTSTR pstrTemp = ::CharNext(lpszValue);
			while( lpszValue < pstrTemp) {
				sItem += *lpszValue++;
			}
		}
		while( *lpszValue > _T('\0') && *lpszValue <= _T(' ') )
		{
			lpszValue = ::CharNext(lpszValue);
		}
		if( *lpszValue++ != _T('=') )
			break;
		while( *lpszValue > _T('\0') && *lpszValue <= _T(' ') )
		{
			lpszValue = ::CharNext(lpszValue);
		}
		if( *lpszValue++ != _T('\'') )
			break;

		while( *lpszValue != _T('\0') && *lpszValue != _T('\'') )
		{
			LPTSTR pstrTemp = ::CharNext(lpszValue);
			while( lpszValue < pstrTemp)
			{
				sValue += *lpszValue++;
			}
		}
		if( *lpszValue++ != _T('\'') )
			break;

		if (sValue.IsEmpty())
		{
			//只有一个图片路径，没有附加格式
			pImage->AddItem(img);
			break;
		}

		if( sItem == _T("file") || sItem == _T("res") )
		{
			if (!img.m_strFile.IsEmpty())
			{
				pImage->AddItem(img);
			}
			img.SetEmpty();
			img.m_strFile = sValue;
		}
		else if( sItem == _T("restype") )
		{
			img.m_strImageResType = sValue;
		}
		else if( sItem == _T("dest") )
		{
			LPCTSTR lpszValue = sValue;
			img.m_rcDst.left = _tcstol(lpszValue, &pstr, 10);
			ASSERT(pstr);
			if (img.m_rcDst.left < 0)
			{
				img.m_bRight = true;
			}
			else if (img.m_rcDst.left == 0)
			{
				LPCTSTR pstrTmp = pstr - 1;
				while (pstrTmp && pstrTmp != lpszValue && (*pstrTmp) >= '0' && (*pstrTmp) <= '9')
				{
					--pstrTmp;
				}
				if (pstrTmp && (*pstrTmp) == '-')
				{
					img.m_bRight = true;
				}
			}
			img.m_rcDst.top = _tcstol(pstr + 1, &pstr, 10);
			ASSERT(pstr);
			if (img.m_rcDst.top < 0)
			{
				img.m_bBottom = true;
			}
			else if (img.m_rcDst.top == 0)
			{
				LPCTSTR pstrTmp = pstr - 1;
				while (pstrTmp && *pstrTmp && (*pstrTmp) >= '0' && (*pstrTmp) <= '9')
				{
					--pstrTmp;
				}
				if (pstrTmp && (*pstrTmp) == '-')
				{
					img.m_bBottom = true;
				}
			}
			img.m_rcDst.right = _tcstol(pstr + 1, &pstr, 10);
			ASSERT(pstr);
			img.m_rcDst.bottom = _tcstol(pstr + 1, &pstr, 10);
			ASSERT(pstr);
		}
		else if( sItem == _T("source") ) 
		{
			img.m_rcSrc.left = _tcstol((LPCTSTR)sValue, &pstr, 10);
			ASSERT(pstr);    
			img.m_rcSrc.top = _tcstol(pstr + 1, &pstr, 10);
			ASSERT(pstr);    
			img.m_rcSrc.right = _tcstol(pstr + 1, &pstr, 10);
			ASSERT(pstr);    
			img.m_rcSrc.bottom = _tcstol(pstr + 1, &pstr, 10);
			ASSERT(pstr);  
		}
		else if( sItem == _T("corner") ) 
		{
			img.m_rcCorner.left = _tcstol((LPCTSTR)sValue, &pstr, 10);
			ASSERT(pstr);    
			img.m_rcCorner.top = _tcstol(pstr + 1, &pstr, 10);
			ASSERT(pstr);
			img.m_rcCorner.right = _tcstol(pstr + 1, &pstr, 10);
			ASSERT(pstr);    
			img.m_rcCorner.bottom = _tcstol(pstr + 1, &pstr, 10);
			ASSERT(pstr);
		}
		else if( sItem == _T("mask") )
		{
			if( sValue[0] == _T('#'))
				img.m_dwMask = _tcstoul((LPCTSTR)sValue + 1, &pstr, 16);
			else
				img.m_dwMask = _tcstoul((LPCTSTR)sValue, &pstr, 16);
		}
		else if( sItem == _T("fade") )
		{
			img.m_byFade = (BYTE)_tcstoul((LPCTSTR)sValue, &pstr, 10);
		}
		else if( sItem == _T("hole") )
		{
			img.m_bHole = (_tcscmp((LPCTSTR)sValue, _T("true")) == 0);
		}
		else if( sItem == _T("xtiled") )
		{
			img.m_bXTiled = (_tcscmp((LPCTSTR)sValue, _T("true")) == 0);
		}
		else if( sItem == _T("ytiled") )
		{
			img.m_bYTiled = (_tcscmp((LPCTSTR)sValue, _T("true")) == 0);
		}
	}
	if (!img.m_strFile.IsEmpty())
	{
		pImage->AddItem(img);
	}
	else if (pImage->GetItemCount() == 0)
	{
		img.m_strFile = strValue;
		pImage->AddItem(img);
	}
	item.value.pImageValue = pImage;
	return true;
}

}