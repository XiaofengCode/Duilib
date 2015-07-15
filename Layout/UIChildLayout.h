#ifndef __UICHILDLAYOUT_H__
#define __UICHILDLAYOUT_H__

#pragma once

namespace DuiLib
{
	class UILIB_API CChildLayoutUI : public CContainerUI
	{
	public:
		CChildLayoutUI();

		void Init();
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void SetChildLayoutXML(CDuiString pXML);
		CDuiString GetChildLayoutXML();
		virtual LPVOID GetInterface(LPCTSTR pstrName);
		virtual LPCTSTR GetClass() const;
		void SetChildStringTableXML( CDuiString pXML );
		CDuiString GetChildStringTableXML();
		void CChildLayoutUI::SetChildStringTableLang( CDuiString strLan );
		CDuiString CChildLayoutUI::GetChildStringTableLang();
	private:
		DuiLib::CDuiString m_pstrXMLFile;
		DuiLib::CDuiString m_strStringTableXMLFile;
		DuiLib::CDuiString m_strLang;
	};
} // namespace DuiLib
#endif // __UICHILDLAYOUT_H__
