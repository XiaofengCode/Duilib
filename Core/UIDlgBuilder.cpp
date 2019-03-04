#include "duipub.h"

namespace DuiLib {

CDialogBuilder::CDialogBuilder() : m_pCallback(NULL), m_pstrtype(NULL), m_pAttrbuteCallback(NULL)
{

}

void CDialogBuilder::SetAttrbuteCallBack( IDialogBuilderAttrbuteCallback * pAttrbuteCallback /*= NULL*/ )
{
	m_pAttrbuteCallback = pAttrbuteCallback;
}

CControlUI* CDialogBuilder::Create( STRINGorID xml, LPCTSTR type /*= NULL*/, IDialogBuilderCallback* pCallback /*= NULL*/, CPaintManagerUI* pManager /*= NULL*/, CControlUI* pParent /*= NULL*/ )
{
	//资源ID为0-65535，两个字节；字符串指针为4个字节
	//字符串以<开头认为是XML字符串，否则认为是XML文件

	if( HIWORD(xml.m_lpstr) != NULL ) {
		if( *(xml.m_lpstr) == _T('<') ) {
			if( !m_xml.Load(xml.m_lpstr) ) return NULL;
		}
		else {
			if( !m_xml.LoadFromFile(xml.m_lpstr) ) return NULL;
		}
	}
	else {
		HRSRC hResource = ::FindResource(CPaintManagerUI::GetResourceDll(), xml.m_lpstr, type);
		if( hResource == NULL ) return NULL;
		HGLOBAL hGlobal = ::LoadResource(CPaintManagerUI::GetResourceDll(), hResource);
		if( hGlobal == NULL ) {
			FreeResource(hResource);
			return NULL;
		}

		m_pCallback = pCallback;
		if( !m_xml.LoadFromMem((BYTE*)::LockResource(hGlobal), ::SizeofResource(CPaintManagerUI::GetResourceDll(), hResource) )) return NULL;
		::FreeResource(hResource);
		m_pstrtype = type;
	}
	return Create(pCallback, pManager, pParent);
}

CControlUI* CDialogBuilder::Create(IDialogBuilderCallback* pCallback, CPaintManagerUI* pManager, CControlUI* pParent)
{
    m_pCallback = pCallback;
    CDuiXmlNode root = m_xml.GetRoot();
	if( !root ) return NULL;
	double S = 1.0;
	if (pManager)
	{
		S = pManager->GetDpiScale();
	}
	if( pManager )
	{
		LPCTSTR pstrClass = NULL;
		LPCTSTR pstrName = NULL;
		LPCTSTR pstrValue = NULL;
		LPTSTR pstr = NULL;

        pstrClass = root.name();
		if( _tcsicmp(pstrClass, _T("Window")) == 0 )
		{
			_ParseWindow(pManager, root);
        }

		for( CDuiXmlNode node = root.first_child() ; node; node = node.next_sibling() )
		{
			pstrClass = node.name();
			if( _tcsicmp(pstrClass, _T("Image")) == 0 ) 
			{
				_ParseImage(node, pManager);
			}
			else if( _tcsicmp(pstrClass, _T("Font")) == 0 )
			{
				_ParseFont(pManager, node);
			}
			else if( _tcsicmp(pstrClass, _T("Default")) == 0 )
			{
				_ParseDefault(node, pManager);
			}
			else if( _tcsicmp(pstrClass, _T("Script")) == 0 )
			{
				_ParseScript(node, pManager);
			}
// 			else if( _tcsicmp(pstrClass, _T("Include")) == 0 )
// 			{
// 				_ParseInclude(node, pManager, pParent);
// 			}
		}
    }
	
    return _ParseControl(&root, pParent, pManager);
}

void CDialogBuilder::_ParseEvent(CDuiXmlNode &node, CPaintManagerUI* pManager, CControlUI* pParent)
{
	if (!pManager)
	{
		return;
	}
	//<Event setFocus="ClickEvent.click">
	//	<click>print("按钮点击")</click>
	//</Event>
	pParent->SetManager(pManager, NULL, false);
// 	char nameBuf[MAX_PATH];
// 	char valueBuf[MAX_PATH];
	std::string sName;
	std::string sValue;
	LuaState* L = pManager->GetLuaState();
	if(!L)
	{
		return;
	}
	try
	{
		for (CDuiXmlAttr attr=node.first_attribute();attr;attr=attr.next_attribute())
		{
			//sName = DuiUtf16ToAscii(attr.name());
			CDuiString strValue(attr.value());
			//sValue = DuiUtf16ToAscii(attr.value());
// 			UTF16To8(nameBuf,(unsigned short*)attr.name(),sizeof(nameBuf));
// 			UTF16To8(valueBuf,(unsigned short*)attr.value(),sizeof(valueBuf));
			//strValue.MakeLower();
			CDuiStringArray arValue = strValue.Split('.');
			if (arValue.GetSize() == 1)
			{
				std::string sFun = DuiUtf16ToAscii(strValue);
				pParent->BindLuaEvent(attr.name(), L->getGlobal(sFun.c_str()));
			}
			else
			{
				std::string sModule = DuiUtf16ToAscii(arValue[0]);
				LuaTable tab=L->require(sModule.c_str());
				if (tab.isValid())
				{
					std::string sFun = DuiUtf16ToAscii(arValue[1]);
					pParent->BindLuaEvent(attr.name(),tab.getTable(sFun.c_str()));
				}
			}
		}

		for( CDuiXmlNode evNode = node.first_child() ; evNode; evNode = evNode.next_sibling() )
		{
// 			UTF16To8(nameBuf,(unsigned short*)evNode.name(),sizeof(nameBuf));
// 			int len=UTF16To8(NULL,(unsigned short*)evNode.text().as_string(),0);
// 			char* buf=(char*)dlmalloc(len+1);
// 			UTF16To8(buf,(unsigned short*)evNode.text().as_string(),len+1);
			pParent->BindLuaEvent(evNode.name(), evNode.text().as_string());
			//dlfree(buf);
		}
	}
	catch(LuaException err)
	{
		OutputDebugStringA(err.what());
		//LOGE("doString error:"<<err.what());
	}	
}

void CDialogBuilder::_ParseDefault(CDuiXmlNode &node, CPaintManagerUI* pManager)
{
	LPCTSTR pstrName = NULL;
	LPCTSTR pstrValue = NULL;

	LPCTSTR pControlName = NULL;
	LPCTSTR pControlValue = NULL;
	xml_object_range<xml_attribute_iterator> attrs = node.attributes();
	for( xml_attribute_iterator attr = attrs.begin(); attr != attrs.end(); attr++ )
	{
		pstrName = attr->name();
		pstrValue = CDuiStringTable::FormatString(pManager, attr->as_string());
		if( _tcsicmp(pstrName, _T("name")) == 0 )
		{
			pControlName = pstrValue;
		}
		else if( _tcsicmp(pstrName, _T("value")) == 0 )
		{
			pControlValue = pstrValue;
		}
	}
	if( pControlName )
	{
		pManager->AddDefaultAttributeList(pControlName, pControlValue);
	}
}

void CDialogBuilder::_ParseScript(CDuiXmlNode &node, CPaintManagerUI* pManager)
{
	LuaState* L = pManager->GetLuaState();
	if (!L)
	{
		return;
	}
	try{
		L->doString(DUI_T2A(node.text().as_string()).c_str());
	}catch(LuaException err){
		OutputDebugStringA(err.what());
	}
}

void CDialogBuilder::_ParseFont(CPaintManagerUI* pManager, CDuiXmlNode &node)
{
	double S = 1.0;
	if (pManager)
	{
		S = pManager->GetDpiScale();
	}
	LPCTSTR pstrName = NULL;
	LPCTSTR pstrValue = NULL;
	LPTSTR pstr = NULL;

	LPCTSTR pFontID = NULL;
	LPCTSTR pFontName = NULL;
	int size = (int)(S * 12);
	bool bold = false;
	bool underline = false;
	bool italic = false;
	bool defaultfont = false;

	LPCTSTR pFontName2 = NULL;
	int size2 = 12;
	bool bold2 = false;
	bool underline2 = false;
	bool italic2 = false;

	xml_object_range<xml_attribute_iterator> attrsFont = node.attributes();
	for( xml_attribute_iterator attr = attrsFont.begin(); attr != attrsFont.end(); attr++ )
	{
		pstrName = attr->name();
		pstrValue = CDuiStringTable::FormatString(pManager, attr->as_string());
		if( _tcsicmp(pstrName, _T("id")) == 0 )
		{
			pFontID = pstrValue;
		}
		else if( _tcsicmp(pstrName, _T("name")) == 0 )
		{
			pFontName = pstrValue;
		}
		else if( _tcsicmp(pstrName, _T("size")) == 0 )
		{
			size = (int)(S *  _tcstol(pstrValue, &pstr, 10));
		}
		else if( _tcsicmp(pstrName, _T("bold")) == 0 )
		{
			bold = (_tcsicmp(pstrValue, _T("true")) == 0);
		}
		else if( _tcsicmp(pstrName, _T("underline")) == 0 )
		{
			underline = (_tcsicmp(pstrValue, _T("true")) == 0);
		}
		else if( _tcsicmp(pstrName, _T("italic")) == 0 )
		{
			italic = (_tcsicmp(pstrValue, _T("true")) == 0);
		}
		else if( _tcsicmp(pstrName, _T("default")) == 0 )
		{
			defaultfont = (_tcsicmp(pstrValue, _T("true")) == 0);
		}
		if( _tcsicmp(pstrName, _T("name2")) == 0 )
		{
			pFontName2 = pstrValue;
		}
		else if( _tcsicmp(pstrName, _T("size2")) == 0 ) 
		{
			size2 = _tcstol(pstrValue, &pstr, 10);
		}
		else if( _tcsicmp(pstrName, _T("bold2")) == 0 )
		{
			bold2 = (_tcsicmp(pstrValue, _T("true")) == 0);
		}
		else if( _tcsicmp(pstrName, _T("underline2")) == 0 )
		{
			underline2 = (_tcsicmp(pstrValue, _T("true")) == 0);
		}
		else if( _tcsicmp(pstrName, _T("italic2")) == 0 )
		{
			italic2 = (_tcsicmp(pstrValue, _T("true")) == 0);
		}
	}

	if( pFontName ) 
	{
		BOOL bAllocID = FALSE;
		if (!pFontID)
		{
			TCHAR* pID = new TCHAR[64];
			_stprintf_s(pID, 64, _T("%lu"), pManager->GetCustomFontCount());
			pFontID = pID;
			bAllocID = TRUE;
		}
		if (pFontName2)
		{
			//支持备用字体
			if (NULL == pManager->AddFont(pFontID, pFontName, size, bold, underline, italic, TRUE))
			{
				if (NULL != pManager->AddFont(pFontID, pFontName2, size2, bold2, underline2, italic2))
				{
					if( defaultfont )
						pManager->SetDefaultFont(pFontName2, size2, bold2, underline2, italic2);
				}
			}
			else
			{
				if( defaultfont )
					pManager->SetDefaultFont(pFontName, size, bold, underline, italic);
			}
		}
		else
		{
			if (NULL != pManager->AddFont(pFontID, pFontName, size, bold, underline, italic))
			{
				if( defaultfont )
					pManager->SetDefaultFont(pFontName, size, bold, underline, italic);
			}
		}
		if (bAllocID)
		{
			delete[] pFontID;
		}

	}
}

CMarkup* CDialogBuilder::GetMarkup()
{
    return &m_xml;
}

bool CDialogBuilder::LoadFile(LPCTSTR file)
{
	return m_xml.LoadFromFile(file);
}

bool CDialogBuilder::LoadString(LPCWSTR str)
{
	return m_xml.Load(str);
}

bool CDialogBuilder::LoadString(LPCSTR str)
{
	return m_xml.LoadFromMem((BYTE*)str, strlen(str)+1, XMLFILE_ENCODING_UTF8);
}

// void CDialogBuilder::GetLastErrorMessage(LPTSTR pstrMessage, SIZE_T cchMax) const
// {
//     return m_xml.GetLastError(pstrMessage, cchMax);
// }
// 
// void CDialogBuilder::GetLastErrorLocation(LPTSTR pstrSource, SIZE_T cchMax) const
// {
//     return m_xml.GetLastErrorLocation(pstrSource, cchMax);
// }

CControlUI* CDialogBuilder::_ParseControl(CDuiXmlNode* pRoot, CControlUI* pParent, CPaintManagerUI* pManager)
{
    IContainerUI* pContainer = NULL;
    CControlUI* pReturn = NULL;
    for( CDuiXmlNode node = pRoot->first_child() ; node; node = node.next_sibling() )
	{
        LPCTSTR pstrClass = node.name();
        if( _tcsicmp(pstrClass, _T("Image")) == 0 ||
			_tcsicmp(pstrClass, _T("Font")) == 0 ||
			_tcsicmp(pstrClass, _T("Default")) == 0 )
			continue;

        CControlUI* pControl = NULL;
        if( _tcsicmp(pstrClass, _T("Include")) == 0 )
		{
			_ParseInclude(node, pManager, pParent);
            continue;
		}
// 		else if( _tcsicmp(pstrClass, _T("Image")) == 0 ) 
// 		{
// 			_ParseImage(node, pManager);
// 		}
// 		else if( _tcsicmp(pstrClass, _T("Font")) == 0 )
// 		{
// 			_ParseFont(pManager, node);
// 		}
// 		else if( _tcsicmp(pstrClass, _T("Default")) == 0 )
// 		{
// 			_ParseDefault(node, pManager);
// 		}
		else if( _tcsicmp(pstrClass, _T("Event")) == 0 )
		{
			_ParseEvent(node, pManager, pParent);
		}
		//树控件XML解析
		else if( _tcsicmp(pstrClass, _T("TreeNode")) == 0 )
		{
			CTreeNodeUI* pNode			= new CTreeNodeUI();
			CTreeNodeUI* pParentNode	= NULL;

			// 若有控件默认配置先初始化默认属性
			if( pManager ) 
			{
				pNode->SetManager(pManager, NULL, false);
				LPCTSTR pDefaultAttributes = pManager->GetDefaultAttributeList(pstrClass);
				if( pDefaultAttributes )
				{
					pNode->ApplyAttributeList(pDefaultAttributes);
				}
			}

			// 解析所有属性并覆盖默认属性
			xml_object_range<xml_attribute_iterator> attrs = node.attributes();
			for( xml_attribute_iterator attr = attrs.begin(); attr != attrs.end(); attr++ )
			{
				LPCTSTR pstrName = attr->name();
				LPCTSTR pstrValue = CDuiStringTable::FormatString(pManager, attr->as_string());
				if (!m_pAttrbuteCallback || m_pAttrbuteCallback->SetAttribute(pNode, pstrName, pstrValue))
				{
					pNode->SetAttribute(pstrName, pstrValue);
				}
			}

			//检索子节点及附加控件
			_ParseControl(&node, pNode, pManager);

			if (pParent)
			{
				pParentNode	= static_cast<CTreeNodeUI*>(pParent->GetInterface(_T("TreeNode")));
				if(pParentNode)
				{
					if(!pParentNode->Add(pNode))
					{
						delete pNode;
						continue;
					}
				}
				else
				{
					CTreeViewUI* pTreeView = static_cast<CTreeViewUI*>(pParent->GetInterface(_T("TreeView")));
					if( pTreeView == NULL ) return NULL;
					if( !pTreeView->Add(pNode) )
					{
						delete pNode;
						continue;
					}
				}
			}
			// Return first item
			if( pReturn == NULL ) pReturn = pNode;
			continue;
		}
        else
		{
            SIZE_T cchLen = _tcslen(pstrClass);
            switch( cchLen )
			{
            case 4:
                if( _tcsicmp(pstrClass, DUI_CTR_EDIT) == 0 )                   pControl = new CEditUI;
                else if( _tcsicmp(pstrClass, DUI_CTR_LIST) == 0 )              pControl = new CListUI;
                else if( _tcsicmp(pstrClass, DUI_CTR_TEXT) == 0 )              pControl = new CTextUI;
                break;
            case 5:
                if( _tcsicmp(pstrClass, DUI_CTR_COMBO) == 0 )                  pControl = new CComboUI;
                else if( _tcsicmp(pstrClass, DUI_CTR_LABEL) == 0 )             pControl = new CLabelUI;
				else if( _tcsicmp(pstrClass, DUI_CTR_FLASH) == 0 )             pControl = new CFlashUI;
                break;
            case 6:
                if( _tcsicmp(pstrClass, DUI_CTR_BUTTON) == 0 )                 pControl = new CButtonUI;
                else if( _tcsicmp(pstrClass, DUI_CTR_OPTION) == 0 )            pControl = new COptionUI;
                else if( _tcsicmp(pstrClass, DUI_CTR_SLIDER) == 0 )            pControl = new CSliderUI;
                break;
            case 7:
                if( _tcsicmp(pstrClass, DUI_CTR_CONTROL) == 0 )                pControl = new CControlUI;
                else if( _tcsicmp(pstrClass, DUI_CTR_ACTIVEX) == 0 )           pControl = new CActiveXUI;
				else if( _tcsicmp(pstrClass, DUI_CTR_GIFANIM) == 0 )           pControl = new CGifAnimUI;
                break;
            case 8:
                if( _tcsicmp(pstrClass, DUI_CTR_PROGRESS) == 0 )               pControl = new CProgressUI;
                else if( _tcsicmp(pstrClass, DUI_CTR_RICHEDIT) == 0 )          pControl = new CRichEditUI;
				else if( _tcsicmp(pstrClass, DUI_CTR_CHECKBOX) == 0 )		  pControl = new CCheckBoxUI;
				else if( _tcsicmp(pstrClass, DUI_CTR_COMBOBOX) == 0 )		  pControl = new CComboBoxUI;
				else if( _tcsicmp(pstrClass, DUI_CTR_DATETIME) == 0 )		  pControl = new CDateTimeUI;
				else if( _tcsicmp(pstrClass, DUI_CTR_TREEVIEW) == 0 )		  pControl = new CTreeViewUI;
                break;
            case 9:
                if( _tcsicmp(pstrClass, DUI_CTR_CONTAINER) == 0 )              pControl = new CContainerUI;
                else if( _tcsicmp(pstrClass, DUI_CTR_TABLAYOUT) == 0 )         pControl = new CTabLayoutUI;
				else if( _tcsicmp(pstrClass, DUI_CTR_SCROLLBAR) == 0 )         pControl = new CScrollBarUI; 
				else if( _tcsicmp(pstrClass, DUI_CTR_IPADDRESS) == 0 )         pControl = new CIPAddressUI; 
                break;
            case 10:
                if( _tcsicmp(pstrClass, DUI_CTR_LISTHEADER) == 0 )             pControl = new CListHeaderUI;
                else if( _tcsicmp(pstrClass, DUI_CTR_TILELAYOUT) == 0 )        pControl = new CTileLayoutUI;
				else if( _tcsicmp(pstrClass, DUI_CTR_WEBBROWSER) == 0 )        pControl = new CWebBrowserUI;
                break;
			case 11:
				if (_tcsicmp(pstrClass, DUI_CTR_CHILDLAYOUT) == 0)			  pControl = new CChildLayoutUI;
				break;
            case 14:
                if( _tcsicmp(pstrClass, DUI_CTR_VERTICALLAYOUT) == 0 )         pControl = new CVerticalLayoutUI;
				else if( _tcsicmp(pstrClass, DUI_CTR_LISTHEADERITEM) == 0 )    pControl = new CListHeaderItemUI;
				else if( _tcsicmp(pstrClass, DUI_CTR_PROGRESSBUTTON) == 0 )    pControl = new CProgressButtonUI;
                break;
			case 15:
				if( _tcsicmp(pstrClass, DUI_CTR_LISTTEXTELEMENT) == 0 )        pControl = new CListTextElementUI;
				if( _tcsicmp(pstrClass, DUI_CTR_SLIDERTABLAYOUT) == 0 )        pControl = new CSliderTabLayoutUI;
                break;
            case 16:
                if( _tcsicmp(pstrClass, DUI_CTR_HORIZONTALLAYOUT) == 0 )       pControl = new CHorizontalLayoutUI;
                else if( _tcsicmp(pstrClass, DUI_CTR_LISTLABELELEMENT) == 0 )  pControl = new CListLabelElementUI;
                break;
            case 20:
                if( _tcsicmp(pstrClass, DUI_CTR_LISTCONTAINERELEMENT) == 0 )   pControl = new CListContainerElementUI;
                break;
            }
            // User-supplied control factory
            if( pControl == NULL ) 
			{
                CStdPtrArray* pPlugins = CPaintManagerUI::GetPlugins();
                LPCREATECONTROL lpCreateControl = NULL;
                for( int i = 0; i < pPlugins->GetSize(); ++i )
				{
                    lpCreateControl = (LPCREATECONTROL)pPlugins->GetAt(i);
                    if( lpCreateControl != NULL ) 
					{
                        pControl = lpCreateControl(pstrClass);
                        if( pControl != NULL )
							break;
                    }
                }
            }
            if( pControl == NULL && m_pCallback != NULL )
			{
                pControl = m_pCallback->CreateControl(pstrClass);
            }
        }

#ifndef _DEBUG
        ASSERT(pControl);
#endif // _DEBUG
			if( pControl == NULL )
			{
#ifdef _DEBUG
				DUITRACE(_T("未知控件:%s"),pstrClass);
#else
				continue;
#endif
			}

        // Add children
           _ParseControl(&node, pControl, pManager);

        // Attach to parent
        // 因为某些属性和父窗口相关，比如selected，必须先Add到父窗口
		if( pParent != NULL )
		{
			CTreeNodeUI* pContainerNode = static_cast<CTreeNodeUI*>(pParent->GetInterface(_T("TreeNode")));
			if(pContainerNode)
				pContainerNode->GetTreeNodeHoriznotal()->Add(pControl);
			else
			{
				if( pContainer == NULL ) pContainer = static_cast<IContainerUI*>(pParent->GetInterface(_T("IContainer")));
				ASSERT(pContainer);
				if( pContainer == NULL ) return NULL;
				if( !pContainer->Add(pControl) ) {
					delete pControl;
					continue;
				}
			}
		}
        // Init default attributes
        if( pManager ) {
            pControl->SetManager(pManager, NULL, false);
            LPCTSTR pDefaultAttributes = pManager->GetDefaultAttributeList(pstrClass);
            if( pDefaultAttributes ) {
                pControl->ApplyAttributeList(pDefaultAttributes);
            }
        }
		// Process attributes
		xml_object_range<xml_attribute_iterator> attrsCtrl = node.attributes();
		for( xml_attribute_iterator attr = attrsCtrl.begin(); attr != attrsCtrl.end(); attr++ )
		{
			LPCTSTR pstrName = attr->name();
			LPCTSTR pstrValue = CDuiStringTable::FormatString(pManager, attr->as_string());
			if (!m_pAttrbuteCallback || m_pAttrbuteCallback->SetAttribute(pControl, pstrName, pstrValue))
			{
				pControl->SetAttribute(pstrName, pstrValue);
			}

		}
        if( pManager )
		{
            pControl->SetManager(NULL, NULL, false);
        }
        // Return first item
        if( pReturn == NULL ) pReturn = pControl;
    }
    return pReturn;
}

void CDialogBuilder::_ParseInclude(CDuiXmlNode &node, CPaintManagerUI* pManager, CControlUI* pParent)
{
	int count = 1;
	LPTSTR pstr = NULL;
	CDuiXmlAttr attrSrc;
	xml_object_range<xml_attribute_iterator> attrs = node.attributes();
	for (xml_attribute_iterator attr = attrs.begin(); attr != attrs.end(); attr++)
	{
		if (_tcsicmp(attr->name(), _T("count")) == 0)
		{
			count = attr->as_int();
		}
		else if (_tcsicmp(attr->name(), _T("source")) == 0)
		{
			attrSrc = *attr;
		}
	}
	if (!attrSrc)
	{
		return;
	}
	for ( int i = 0; i < count; i++ )
	{
		CDialogBuilder builder;
		if( m_pstrtype != NULL )
		{ // 使用资源dll，从资源中读取
			WORD id = (WORD)attrSrc.as_int();
			builder.Create((UINT)id, m_pstrtype, m_pCallback, pManager, pParent);
		}
		else
		{
			builder.Create(attrSrc.as_string(), (UINT)0, m_pCallback, pManager, pParent);
		}
	}
}

void CDialogBuilder::_ParseWindow(CPaintManagerUI* pManager, CDuiXmlNode &root)
{
	LPCTSTR pstrClass = NULL;
	LPCTSTR pstrName = NULL;
	LPCTSTR pstrValue = NULL;
	LPTSTR pstr = NULL;
	CDuiString strStringTable, strLang;

	double S = 1.0;
	if (pManager)
	{
		S = pManager->GetDpiScale();
	}
	if( !pManager->GetPaintWindow() )
	{
		return;
	}
	//int nAttributes = attrsRoot.;
	strStringTable = root.attribute(_T("stringtable")).as_string();
	xml_object_range<xml_attribute_iterator> attrsRoot = root.attributes();
	for( xml_attribute_iterator attr = attrsRoot.begin(); attr != attrsRoot.end(); attr++ )
	{
		if (_tcsicmp(attr->name(), _T("stringtable")) == 0)
		{
			strStringTable = attr->as_string();
		}
		else if (_tcsicmp(attr->name(), _T("stringtablelang")) == 0)
		{
			strLang = attr->as_string();
		}
	}
	if (strStringTable.GetLength())
	{
		CDuiStringTable& st = pManager->GetStringTable();
		pManager->GetStringTable().Load((LPCTSTR)strStringTable, 0, strLang);
	}

	for( xml_attribute_iterator attr = attrsRoot.begin(); attr != attrsRoot.end(); attr++ )
	{
		pstrName = attr->name();
		pstrValue = CDuiStringTable::FormatString(pManager, attr->as_string());
		if( _tcsicmp(pstrName, _T("size")) == 0 )
		{
			LPTSTR pstr = NULL;
			int cx = (int)(S * _tcstol(pstrValue, &pstr, 10));
			ASSERT(pstr);    
			int cy = (int)(S * _tcstol(pstr + 1, &pstr, 10));
			ASSERT(pstr); 
			pManager->SetInitSize(cx, cy);
		} 
		else if( _tcsicmp(pstrName, _T("sizebox")) == 0 )
		{
			RECT rcSizeBox = { 0 };
			LPTSTR pstr = NULL;
			rcSizeBox.left = (LONG)(S * _tcstol(pstrValue, &pstr, 10));
			ASSERT(pstr);    
			rcSizeBox.top = (LONG)(S * _tcstol(pstr + 1, &pstr, 10));
			ASSERT(pstr);    
			rcSizeBox.right = (LONG)(S * _tcstol(pstr + 1, &pstr, 10));
			ASSERT(pstr);    
			rcSizeBox.bottom = (LONG)(S * _tcstol(pstr + 1, &pstr, 10));
			ASSERT(pstr);    
			pManager->SetSizeBox(rcSizeBox);
		}
		else if( _tcsicmp(pstrName, _T("caption")) == 0 ) 
		{
			RECT rcCaption = { 0 };
			LPTSTR pstr = NULL;
			rcCaption.left = (LONG)(S * _tcstol(pstrValue, &pstr, 10));
			ASSERT(pstr);    
			rcCaption.top = (LONG)(S * _tcstol(pstr + 1, &pstr, 10));
			ASSERT(pstr);    
			rcCaption.right = (LONG)(S * _tcstol(pstr + 1, &pstr, 10));
			ASSERT(pstr);    
			rcCaption.bottom = (LONG)(S * _tcstol(pstr + 1, &pstr, 10));
			ASSERT(pstr);    
			pManager->SetCaptionRect(rcCaption);
		}
		else if( _tcsicmp(pstrName, _T("roundcorner")) == 0 )
		{
			LPTSTR pstr = NULL;
			int cx = (int)(S * _tcstol(pstrValue, &pstr, 10));
			ASSERT(pstr);    
			int cy = (int)(S * _tcstol(pstr + 1, &pstr, 10));
			ASSERT(pstr); 
			pManager->SetRoundCorner(cx, cy);
		} 
		else if( _tcsicmp(pstrName, _T("mininfo")) == 0 )
		{
			LPTSTR pstr = NULL;
			int cx = (int)(S * _tcstol(pstrValue, &pstr, 10));
			ASSERT(pstr);    
			int cy = (int)(S * _tcstol(pstr + 1, &pstr, 10));
			ASSERT(pstr); 
			pManager->SetMinInfo(cx, cy);
		}
		else if( _tcsicmp(pstrName, _T("maxinfo")) == 0 )
		{
			LPTSTR pstr = NULL;
			int cx = (int)(S * _tcstol(pstrValue, &pstr, 10));
			ASSERT(pstr);    
			int cy = (int)(S * _tcstol(pstr + 1, &pstr, 10));
			ASSERT(pstr); 
			pManager->SetMaxInfo(cx, cy);
		}
		else if( _tcsicmp(pstrName, _T("showdirty")) == 0 )
		{
			pManager->SetShowUpdateRect(_tcsicmp(pstrValue, _T("true")) == 0);
		} 
		else if( _tcsicmp(pstrName, _T("alpha")) == 0 )
		{
			pManager->SetTransparent(_ttoi(pstrValue));
		} 
		else if( _tcsicmp(pstrName, _T("bktrans")) == 0 )
		{
			pManager->SetBackgroundTransparent(_tcsicmp(pstrValue, _T("true")) == 0);
		} 
		else if( _tcsicmp(pstrName, _T("disabledfontcolor")) == 0 )
		{
			if( *pstrValue == _T('#'))
				pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			pManager->SetDefaultDisabledColor(clrColor);
		} 
		else if( _tcsicmp(pstrName, _T("defaultfontcolor")) == 0 )
		{
			if( *pstrValue == _T('#'))
				pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			pManager->SetDefaultFontColor(clrColor);
		}
		else if( _tcsicmp(pstrName, _T("linkfontcolor")) == 0 )
		{
			if( *pstrValue == _T('#')) 
				pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			pManager->SetDefaultLinkFontColor(clrColor);
		} 
		else if( _tcsicmp(pstrName, _T("linkhoverfontcolor")) == 0 )
		{
			if( *pstrValue == _T('#')) 
				pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			pManager->SetDefaultLinkHoverFontColor(clrColor);
		} 
		else if( _tcsicmp(pstrName, _T("selectedcolor")) == 0 )
		{
			if( *pstrValue == _T('#'))
				pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			pManager->SetDefaultSelectedBkColor(clrColor);
		} 
		else if( _tcsicmp(pstrName, _T("shadowsize")) == 0 )
		{
			pManager->GetShadow()->SetSize((int)(S * _ttoi(pstrValue)));
		}
		else if( _tcsicmp(pstrName, _T("shadowsharpness")) == 0 )
		{
			pManager->GetShadow()->SetSharpness(_ttoi(pstrValue));
		}
		else if( _tcsicmp(pstrName, _T("shadowdarkness")) == 0 )
		{
			pManager->GetShadow()->SetDarkness(_ttoi(pstrValue));
		}
		else if( _tcsicmp(pstrName, _T("shadowposition")) == 0 )
		{
			LPTSTR pstr = NULL;
			int cx = (int)(S * _tcstol(pstrValue, &pstr, 10));
			ASSERT(pstr);    
			int cy = (int)(S * _tcstol(pstr + 1, &pstr, 10));
			ASSERT(pstr); 
			pManager->GetShadow()->SetPosition(cx, cy);
		}
		else if( _tcsicmp(pstrName, _T("shadowcolor")) == 0 ) 
		{
			if( *pstrValue == _T('#')) 
				pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			pManager->GetShadow()->SetColor(clrColor);
		}
		else if( _tcsicmp(pstrName, _T("shadowcorner")) == 0 )
		{
			RECT rcCorner = { 0 };
			LPTSTR pstr = NULL;
			rcCorner.left = (int)(S * _tcstol(pstrValue, &pstr, 10));
			ASSERT(pstr);    
			rcCorner.top = (int)(S * _tcstol(pstr + 1, &pstr, 10));
			ASSERT(pstr);    
			rcCorner.right = (int)(S * _tcstol(pstr + 1, &pstr, 10));
			ASSERT(pstr);    
			rcCorner.bottom = (int)(S * _tcstol(pstr + 1, &pstr, 10));
			ASSERT(pstr);    
			pManager->GetShadow()->SetShadowCorner(rcCorner);
		}
		else if( _tcsicmp(pstrName, _T("shadowimage")) == 0 )
		{
			pManager->GetShadow()->SetImage(pstrValue);
		}
		else if( _tcsicmp(pstrName, _T("showshadow")) == 0 )
		{
			pManager->GetShadow()->ShowShadow(_tcsicmp(pstrValue, _T("true")) == 0);
		} 
		else if( _tcsicmp(pstrName, _T("gdiplustext")) == 0 )
		{
			pManager->SetUseGdiplusText(_tcsicmp(pstrValue, _T("true")) == 0);
		} 
		else if(_tcsicmp(pstrName, _T("trayiconid")) == 0 )
		{
			if(_ttoi(pstrValue) > 0)
				pManager->GetTrayObject()->CreateTrayIcon(pManager, _ttoi(pstrValue));
			else
				pManager->GetTrayObject()->CreateTrayIcon(pManager, pstrValue);
		}
		else if(_tcsicmp(pstrName, _T("traytiptext")) == 0 )
		{
			pManager->GetTrayObject()->SetTooltipText(pstrValue);
		}
		else if(_tcsicmp(pstrName, _T("titile")) == 0 )
		{
			pManager->SetWindowTitile(pstrValue);
		}
		else if( _tcsicmp(pstrName, _T("showfocusdot")) == 0 )
		{
			pManager->SetShowFocusDot(_tcsicmp(pstrValue, _T("true")) == 0);
		} 
		else if( _tcsicmp(pstrName, _T("defaultfocusdotcolor")) == 0 )
		{
			if( *pstrValue == _T('#')) 
				pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			pManager->SetDefaultFocusDotColor(clrColor);
		} 
	}
}

void CDialogBuilder::_ParseImage(CDuiXmlNode &node, CPaintManagerUI* pManager)
{
	LPCTSTR pstrName = NULL;
	LPCTSTR pstrValue = NULL;
	LPTSTR pstr = NULL;

	LPCTSTR pImageName = NULL;
	LPCTSTR pImageResType = NULL;
	DWORD mask = 0;
	xml_object_range<xml_attribute_iterator> attrs = node.attributes();
	for( xml_attribute_iterator attr = attrs.begin(); attr != attrs.end(); attr++ )
	{
		pstrName = attr->name();
		pstrValue = CDuiStringTable::FormatString(pManager, attr->as_string());
		if( _tcsicmp(pstrName, _T("name")) == 0 )
		{
			pImageName = pstrValue;
		}
		else if( _tcsicmp(pstrName, _T("restype")) == 0 )
		{
			pImageResType = pstrValue;
		}
		else if( _tcsicmp(pstrName, _T("mask")) == 0 ) 
		{
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			mask = _tcstoul(pstrValue, &pstr, 16);
		}
	}
	if( pImageName ) pManager->AddImage(pImageName, pImageResType, mask);
}

} // namespace DuiLib
