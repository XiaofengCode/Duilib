#pragma once
#include "Core/UIMarkup.h"
namespace DuiLib{

#define STRINGID_ID			_T("@IDS_")
#define STRINGID_ID_2		_T("#IDS_")


	class UILIB_API CDuiStringTable
	{
	public:
		CDuiStringTable();
		bool Load(STRINGorID xml, LPCTSTR type, LPCTSTR lpszLang);
		bool LoadFromFile(LPCTSTR lpszFile, LPCTSTR lpszLang);
		LPCTSTR GetString(LPCTSTR lpszID);
		LPCTSTR FormatString(LPCTSTR lpszSource);
		static LPCTSTR FormatString(CPaintManagerUI* pManager, LPCTSTR lpszSource);
		bool IsValid();
	private:
		bool _Failed(LPCTSTR pstrError, LPCTSTR pstrLocation = NULL);
		void Parse();
		CMarkup m_xml;
		LPCTSTR m_pstrtype;
		//CDuiStringMap m_mapStringHash;
		CStdStringPtrMap m_mapStringHash;
		CDuiString m_strLan;
	};

}