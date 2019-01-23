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

	层级相关：
		back/bk/bg	0x00000001
		fore		0x00000002
		normal		0x00000004
		border		0x00000008
		left		0x00000010
		top			0x00000020
		right		0x00000040
		bottom		0x00000080
	
	附加：
		min
		max
		relative

	可修饰属性：
		color
		pos
		padding
		colorhsl
		size
		style
		round
		width
		height
		text
		textcolor

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
	CAttributeManager::ParseIntOrString
};

void CAttributeManager::AddBindStatus(LPCTSTR lpszAttrPrefix, DWORD dwStatus)
{
	CDuiString strPrefix(lpszAttrPrefix);
	strPrefix.MakeLower();
	m_mapStatuses[strPrefix] = dwStatus;
}

// bool CAttributeManager::AddBindAttribute(LPCTSTR lpszAttr, ValueType type, LPCTSTR lpszDefaultValue)
// {
// 	CDuiString strAttr(lpszAttr);
// 	strAttr.MakeLower();
// 	ATTR_ITEM item;
// 	if (type >= TypeMax)
// 	{
// 		return false;
// 	}
// 	item.type = type;
// 	item.strValue = item.strDefaultValue = lpszDefaultValue;
// 
// 	DWORD dwStatus = 0;
// 	LPCTSTR lpszBaseAttr = ParseStatus(strAttr, dwStatus);
// 	CMapAttrs::iterator itor = m_mapAttrs.find(lpszBaseAttr);
// 	if (itor == m_mapAttrs.end())
// 	{
// 		m_mapAttrs[lpszBaseAttr] = item;
// 	}
// 	bool bRet = ParseAttributeValue(strAttr, lpszDefaultValue);
// 	if (itor == m_mapAttrs.end() && dwStatus)
// 	{
// 		m_mapAttrs.erase(lpszBaseAttr);
// 	}
// 	return bRet;
// }

bool CAttributeManager::AddBindAttribute(LPCTSTR lpszAttr, ValueType type, void* pBind)
{
	CDuiString strAttr(lpszAttr);
	strAttr.MakeLower();
	ATTR_ITEM item;
	if (type >= TypeMax)
	{
		return false;
	}
	item.type = type;

	DWORD dwStatus = 0;
	LPCTSTR lpszBaseAttr = ParseStatus(strAttr, dwStatus);
	CMapAttrs::iterator itor = m_mapAttrs.find(lpszBaseAttr);
	bool bRet = true;
	if (itor == m_mapAttrs.end())
	{
		m_mapAttrs[lpszBaseAttr] = item;
		bRet = false;
	}
	else
	{
		if (lpszBaseAttr == lpszAttr)
		{
			itor->second.realtype = TypeUnknown;
			itor->second.value.pValue = pBind;
		}
		else
		{
			CDuiString strAttr;
			strAttr.SmallFormat(_T("%x%s"), dwStatus, lpszBaseAttr);
			CMapAttrs::iterator itor2 = m_mapAttrs.find(strAttr);
			if (itor2 == m_mapAttrs.end())
			{
				ATTR_ITEM attr = itor->second;
				attr.realtype = TypeUnknown;
				attr.value.pValue = pBind;
				m_mapAttrs[strAttr] = attr;
			}
			else
			{
				itor2->second.realtype = TypeUnknown;
				itor2->second.value.pValue = pBind;
			}
		}
	}
	if (itor == m_mapAttrs.end() && dwStatus)
	{
		m_mapAttrs.erase(lpszBaseAttr);
	}
	else
	{
		bRet = true;
	}
	return bRet;
}

bool CAttributeManager::SetAttribute(LPCTSTR lpszAttr, LPCTSTR lpszValue)
{
	CDuiString strAttr(lpszAttr);
	strAttr.MakeLower();
	return ParseAttributeValue(strAttr, lpszValue, 0);
}

bool CAttributeManager::ParseAttributeValue(LPCTSTR lpszAttr, LPCTSTR lpszValue, DWORD dwStatus /*= 0*/)
{
	LPCTSTR lpszBaseAttr = ParseStatus(lpszAttr, dwStatus);
	CMapAttrs::iterator itor = m_mapAttrs.find(lpszBaseAttr);
	if (itor == m_mapAttrs.end())
	{
		return false;
	}
	if (lpszBaseAttr == lpszAttr)
	{
		itor->second.realtype = TypeUnknown;
		itor->second.strValue = lpszValue;
		if (!pfnParseFunctions[itor->second.type](lpszValue, itor->second))
		{
			return false;
		}
	}
	else
	{
		CDuiString strAttr;
		strAttr.SmallFormat(_T("%x%s"), dwStatus, lpszBaseAttr);
		CMapAttrs::iterator itor2 = m_mapAttrs.find(strAttr);
		if (itor2 == m_mapAttrs.end())
		{
			ATTR_ITEM attr = itor->second;
			attr.realtype = TypeUnknown;
			attr.strValue = lpszValue;
			if (!pfnParseFunctions[itor->second.type](lpszValue, attr))
			{
				return false;
			}
			m_mapAttrs[strAttr] = attr;
			return true;
		}
		else
		{
			itor2->second.realtype = TypeUnknown;
			itor2->second.strValue = lpszValue;
			if (!pfnParseFunctions[itor2->second.type](lpszValue, itor2->second))
			{
				return false;
			}
			return true;
		}
	}
	return true;
}

LPCTSTR CAttributeManager::ParseStatus(LPCTSTR lpszAttr, DWORD& dwStatus)
{
	CMapAttrs::iterator itor = m_mapAttrs.find(lpszAttr);
	if (itor == m_mapAttrs.end())
	{
		//没找到就找前缀
		CDuiString strPrefix(lpszAttr);
		int nLen = _tcslen(lpszAttr);
		for (CMapStatus::iterator itor2 = m_mapStatuses.begin(); itor2 != m_mapStatuses.end(); itor2++)
		{
			if (_tcsnicmp(lpszAttr, itor2->first, itor2->first.GetLength()) == 0 && nLen != itor2->first.GetLength())
			{
				dwStatus|= itor2->second;
				return ParseStatus(lpszAttr + itor2->first.GetLength(), dwStatus);
			}
		}
	}
	return lpszAttr;
}

bool CAttributeManager::GetAttributeItem(LPCTSTR lpszAttr, ATTR_ITEM& item, DWORD dwStatus /*= 0*/)
{
	LPCTSTR lpszBaseAttr = ParseStatus(lpszAttr, dwStatus);
	CDuiString strAttr;
	strAttr.SmallFormat(_T("%x%s"), dwStatus, lpszBaseAttr);
	CMapAttrs::iterator itor2 = m_mapAttrs.find(strAttr);
	if (itor2 == m_mapAttrs.end())
	{
		return false;
	}
	item = itor2->second;
	return true;
}

bool CAttributeManager::ParseInt(LPCTSTR lpszValue, ATTR_ITEM& item)
{
	LPTSTR p;
	int i = _tcstol(lpszValue, &p, 10);
	if (p == lpszValue)
	{
		return false;
	}
	item.realtype = TypeInt;
	*item.value.pIntValue = i;
	return true;
}

bool CAttributeManager::ParseColor(LPCTSTR lpszValue, ATTR_ITEM& item)
{
	if( *lpszValue == _T('#')) lpszValue = ::CharNext(lpszValue);
	LPTSTR pstr = NULL;
	DWORD clrColor = _tcstoul(lpszValue, &pstr, 16);
	if (pstr == lpszValue)
	{
		return false;
	}
	item.realtype = TypeColor;
	*item.value.pDwordValue = clrColor;
	return true;
}

bool CAttributeManager::ParseRect(LPCTSTR lpszValue, ATTR_ITEM& item)
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
	*item.value.pRectValue = rc;
	return true;
}

bool CAttributeManager::ParseString(LPCTSTR lpszValue, ATTR_ITEM& item)
{
	item.realtype = TypeString;
	item.strValue = lpszValue;
	return true;
}

bool CAttributeManager::ParseBool(LPCTSTR lpszValue, ATTR_ITEM& item)
{
	if (_tcsicmp(lpszValue, _T("true")) == 0
		|| _tcsicmp(lpszValue, _T("yes")) == 0)
	{
		item.realtype = TypeBool;
		*item.value.pBoolValue = true;
		return true;
	}
	item.realtype = TypeBool;
	*item.value.pBoolValue = false;
	return true;
}

bool CAttributeManager::ParsePoint(LPCTSTR lpszValue, ATTR_ITEM& item)
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
	return true;
}

bool CAttributeManager::ParsePercent(LPCTSTR lpszValue, ATTR_ITEM& item)
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

bool CAttributeManager::ParseIntOrPercent(LPCTSTR lpszValue, ATTR_ITEM& item)
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

bool CAttributeManager::ParseIntOrRect(LPCTSTR lpszValue, ATTR_ITEM& item)
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

bool CAttributeManager::ParseIntOrString(LPCTSTR lpszValue, ATTR_ITEM& item)
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
}