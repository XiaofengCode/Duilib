#include "stdafx.h"
#include "UIChildLayout.h"

namespace DuiLib
{
	CChildLayoutUI::CChildLayoutUI()
	{

	}

	void CChildLayoutUI::Init()
	{
		if (!m_pstrXMLFile.IsEmpty())
		{
			CDialogBuilder builder;
			CContainerUI* pChildWindow = static_cast<CContainerUI*>(builder.Create(m_pstrXMLFile.GetData(), m_strStringTableXMLFile.GetData(), m_strLang, (UINT)0, NULL, m_pManager));
			if (pChildWindow)
			{
				this->Add(pChildWindow);
			}
			else
			{
				this->RemoveAll();
			}
		}
	}

	void CChildLayoutUI::SetAttribute( LPCTSTR pstrName, LPCTSTR pstrValue )
	{
		if( _tcscmp(pstrName, _T("xmlfile")) == 0 )
			SetChildLayoutXML(pstrValue);
		else if( _tcscmp(pstrName, _T("stringtablefile")) == 0 )
			SetChildStringTableXML(pstrValue);
		else if( _tcscmp(pstrName, _T("stringtablelang")) == 0 )
			SetChildStringTableLang(pstrValue);
		else
			CContainerUI::SetAttribute(pstrName,pstrValue);
	}

	void CChildLayoutUI::SetChildLayoutXML( CDuiString pXML )
	{
		m_pstrXMLFile=pXML;
	}

	CDuiString CChildLayoutUI::GetChildLayoutXML()
	{
		return m_pstrXMLFile;
	}

	void CChildLayoutUI::SetChildStringTableXML( CDuiString pXML )
	{
		m_strStringTableXMLFile=pXML;
	}

	CDuiString CChildLayoutUI::GetChildStringTableXML()
	{
		return m_strStringTableXMLFile;
	}

	void CChildLayoutUI::SetChildStringTableLang( CDuiString strLan )
	{
		m_strLang=strLan;
	}

	CDuiString CChildLayoutUI::GetChildStringTableLang()
	{
		return m_strLang;
	}

	LPVOID CChildLayoutUI::GetInterface( LPCTSTR pstrName )
	{
		if( _tcscmp(pstrName, DUI_CTR_CHILDLAYOUT) == 0 ) return static_cast<CChildLayoutUI*>(this);
		return CControlUI::GetInterface(pstrName);
	}

	LPCTSTR CChildLayoutUI::GetClass() const
	{
		return _T("ChildLayoutUI");
	}
} // namespace DuiLib
