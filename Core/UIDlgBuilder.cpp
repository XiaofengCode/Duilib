#include "StdAfx.h"

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
	//��ԴIDΪ0-65535�������ֽڣ��ַ���ָ��Ϊ4���ֽ�
	//�ַ�����<��ͷ��Ϊ��XML�ַ�����������Ϊ��XML�ļ�

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
    CMarkupNode root = m_xml.GetRoot();
	if( !root.IsValid() ) return NULL;
	double S = 1.0;
	if (pManager)
	{
		S = pManager->GetDpiScale();
	}
	if( pManager )
	{
		LPCTSTR pstrClass = NULL;
		int nAttributes = 0;
		LPCTSTR pstrName = NULL;
		LPCTSTR pstrValue = NULL;
		LPTSTR pstr = NULL;

        pstrClass = root.GetName();
		if( _tcsicmp(pstrClass, _T("Window")) == 0 )
		{
			CDuiString strStringTable, strLang;
            if( pManager->GetPaintWindow() )
			{
                int nAttributes = root.GetAttributeCount();
				strStringTable = root.GetAttributeValue(_T("stringtable"));
				if (strStringTable.GetLength())
				{
					strLang = root.GetAttributeValue(_T("stringtablelang"));
					CDuiStringTable& st = pManager->GetStringTable();
					pManager->GetStringTable().Load((LPCTSTR)strStringTable, 0, strLang);
				}

                for( int i = 0; i < nAttributes; i++ )
				{
                    pstrName = root.GetAttributeName(i);
                    pstrValue = CDuiStringTable::FormatString(pManager, root.GetAttributeValue(i));
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
                }
            }
        }

		for( CMarkupNode node = root.GetChild() ; node.IsValid(); node = node.GetSibling() )
		{
			pstrClass = node.GetName();
			if( _tcsicmp(pstrClass, _T("Image")) == 0 ) 
			{
				nAttributes = node.GetAttributeCount();
				LPCTSTR pImageName = NULL;
				LPCTSTR pImageResType = NULL;
				DWORD mask = 0;
				for( int i = 0; i < nAttributes; i++ )
				{
					pstrName = node.GetAttributeName(i);
					pstrValue = CDuiStringTable::FormatString(pManager, node.GetAttributeValue(i));
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
			else if( _tcsicmp(pstrClass, _T("Font")) == 0 )
			{
				nAttributes = node.GetAttributeCount();
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

				for( int i = 0; i < nAttributes; i++ ) 
				{
					pstrName = node.GetAttributeName(i);
					pstrValue = CDuiStringTable::FormatString(pManager, node.GetAttributeValue(i));
					if( _tcsicmp(pstrName, _T("name")) == 0 )
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
					if (pFontName2)
					{
						//֧�ֱ�������
						if (NULL == pManager->AddFont(pFontName, size, bold, underline, italic, TRUE))
						{
							if (NULL != pManager->AddFont(pFontName2, size2, bold2, underline2, italic2))
							{
								if( defaultfont ) pManager->SetDefaultFont(pFontName2, size2, bold2, underline2, italic2);
							}
						}
						else
						{
							if( defaultfont ) pManager->SetDefaultFont(pFontName, size, bold, underline, italic);
						}
					}
					else
					{
						if (NULL != pManager->AddFont(pFontName, size, bold, underline, italic))
						{
							if( defaultfont ) pManager->SetDefaultFont(pFontName, size, bold, underline, italic);
						}
					}

				}
			}
			else if( _tcsicmp(pstrClass, _T("Default")) == 0 )
			{
				nAttributes = node.GetAttributeCount();
				LPCTSTR pControlName = NULL;
				LPCTSTR pControlValue = NULL;
				for( int i = 0; i < nAttributes; i++ )
				{
					pstrName = node.GetAttributeName(i);
					pstrValue = CDuiStringTable::FormatString(pManager, node.GetAttributeValue(i));
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
		}
    }
	
    return _Parse(&root, pParent, pManager);
}

CMarkup* CDialogBuilder::GetMarkup()
{
    return &m_xml;
}

void CDialogBuilder::GetLastErrorMessage(LPTSTR pstrMessage, SIZE_T cchMax) const
{
    return m_xml.GetLastErrorMessage(pstrMessage, cchMax);
}

void CDialogBuilder::GetLastErrorLocation(LPTSTR pstrSource, SIZE_T cchMax) const
{
    return m_xml.GetLastErrorLocation(pstrSource, cchMax);
}

CControlUI* CDialogBuilder::_Parse(CMarkupNode* pRoot, CControlUI* pParent, CPaintManagerUI* pManager)
{
    IContainerUI* pContainer = NULL;
    CControlUI* pReturn = NULL;
    for( CMarkupNode node = pRoot->GetChild() ; node.IsValid(); node = node.GetSibling() )
	{
        LPCTSTR pstrClass = node.GetName();
        if( _tcsicmp(pstrClass, _T("Image")) == 0 ||
			_tcsicmp(pstrClass, _T("Font")) == 0 ||
			_tcsicmp(pstrClass, _T("Default")) == 0 )
			continue;

        CControlUI* pControl = NULL;
        if( _tcsicmp(pstrClass, _T("Include")) == 0 )
		{
            if( !node.HasAttributes() ) 
			{
				continue;
			}
            int count = 1;
            LPTSTR pstr = NULL;
            TCHAR szValue[500] = { 0 };
            SIZE_T cchLen = lengthof(szValue) - 1;
            if ( node.GetAttributeValue(_T("count"), szValue, cchLen) )
			{
                count = _tcstol(szValue, &pstr, 10);
			}
            cchLen = lengthof(szValue) - 1;
            if ( !node.GetAttributeValue(_T("source"), szValue, cchLen) ) 
			{
				continue;
			}
            for ( int i = 0; i < count; i++ )
			{
                CDialogBuilder builder;
                if( m_pstrtype != NULL )
				{ // ʹ����Դdll������Դ�ж�ȡ
                    WORD id = (WORD)_tcstol(szValue, &pstr, 10); 
                    pControl = builder.Create((UINT)id, m_pstrtype, m_pCallback, pManager, pParent);
                }
                else
				{
                    pControl = builder.Create((LPCTSTR)szValue, (UINT)0, m_pCallback, pManager, pParent);
                }
            }
            continue;
        }
		//���ؼ�XML����
		else if( _tcsicmp(pstrClass, _T("TreeNode")) == 0 )
		{
			CTreeNodeUI* pParentNode	= static_cast<CTreeNodeUI*>(pParent->GetInterface(_T("TreeNode")));
			CTreeNodeUI* pNode			= new CTreeNodeUI();
			if(pParentNode)
			{
				if(!pParentNode->Add(pNode))
				{
					delete pNode;
					continue;
				}
			}

			// ���пؼ�Ĭ�������ȳ�ʼ��Ĭ������
			if( pManager ) 
			{
				pNode->SetManager(pManager, NULL, false);
				LPCTSTR pDefaultAttributes = pManager->GetDefaultAttributeList(pstrClass);
				if( pDefaultAttributes )
				{
					pNode->ApplyAttributeList(pDefaultAttributes);
				}
			}

			// �����������Բ�����Ĭ������
			if( node.HasAttributes() )
			{
				TCHAR szValue[500] = { 0 };
				SIZE_T cchLen = lengthof(szValue) - 1;
				// Set ordinary attributes
				int nAttributes = node.GetAttributeCount();
				for( int i = 0; i < nAttributes; i++ )
				{
					pNode->SetAttribute(node.GetAttributeName(i), CDuiStringTable::FormatString(pManager, node.GetAttributeValue(i)));
				}
			}

			//�����ӽڵ㼰���ӿؼ�
			if(node.HasChildren())
			{
				CControlUI* pSubControl = _Parse(&node, pNode, pManager);
				if(pSubControl && _tcsicmp(pSubControl->GetClass(),_T("TreeNodeUI")) != 0)
				{
// 					pSubControl->SetFixedWidth(30);
// 					CHorizontalLayoutUI* pHorz = pNode->GetTreeNodeHoriznotal();
// 					pHorz->Add(new CEditUI());
// 					continue;
				}
			}

			if(!pParentNode)
			{
				CTreeViewUI* pTreeView = static_cast<CTreeViewUI*>(pParent->GetInterface(_T("TreeView")));
				ASSERT(pTreeView);
				if( pTreeView == NULL ) return NULL;
				if( !pTreeView->Add(pNode) )
				{
					delete pNode;
					continue;
				}
			}
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
                        if( pControl != NULL ) break;
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
				DUITRACE(_T("δ֪�ؼ�:%s"),pstrClass);
#else
				continue;
#endif
			}

        // Add children
        if( node.HasChildren() )
		{
            _Parse(&node, pControl, pManager);
        }
        // Attach to parent
        // ��ΪĳЩ���Ժ͸�������أ�����selected��������Add��������
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
        if( node.HasAttributes() ) {
            TCHAR szValue[500] = { 0 };
            SIZE_T cchLen = lengthof(szValue) - 1;
            // Set ordinary attributes
            int nAttributes = node.GetAttributeCount();
            for( int i = 0; i < nAttributes; i++ ) {
				if (m_pAttrbuteCallback){
					if (m_pAttrbuteCallback->SetAttribute(pControl, node.GetAttributeName(i), CDuiStringTable::FormatString(pManager, node.GetAttributeValue(i))))
					{
						pControl->SetAttribute(node.GetAttributeName(i), CDuiStringTable::FormatString(pManager, node.GetAttributeValue(i)));
					}
				}
				else{
					pControl->SetAttribute(node.GetAttributeName(i), CDuiStringTable::FormatString(pManager, node.GetAttributeValue(i)));
				}
            }
        }
        if( pManager ) {
            pControl->SetManager(NULL, NULL, false);
        }
        // Return first item
        if( pReturn == NULL ) pReturn = pControl;
    }
    return pReturn;
}

} // namespace DuiLib
