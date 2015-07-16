#pragma once
#include "Core/UIMarkup.h"
namespace DuiLib{

#define STRINGID_ID		_T("@IDS_")


	class UILIB_API CDuiStringTable
	{
	public:
		CDuiStringTable();
		bool Load(STRINGorID xml, LPCTSTR type, LPCTSTR lpszLang);
		LPCTSTR GetString(LPCTSTR lpszID);
		LPCTSTR FormatString(LPCTSTR lpszSource);
		static LPCTSTR FormatString(CPaintManagerUI* pManager, LPCTSTR lpszSource);
	private:
		void Parse();
		CMarkup m_xml;
		LPCTSTR m_pstrtype;
		//CDuiStringMap m_mapStringHash;
		CStdStringPtrMap m_mapStringHash;
		CDuiString m_strLan;
	};

}