#pragma once
namespace DuiLib{

#define STRINGID_ID		_T("@IDS_")

	struct STRINGMAPITEM
	{
		CDuiString Key;
		CDuiString Value;
		struct STRINGMAPITEM* pPrev;
		struct STRINGMAPITEM* pNext;
	};

	class UILIB_API CDuiStringMap
	{
	public:
		CDuiStringMap(int nSize = 83);
		~CDuiStringMap();

		void Resize(int nSize = 83);
		LPCTSTR Find(LPCTSTR key, bool optimize = true) const;
		bool Insert(LPCTSTR key, LPCTSTR value);
		LPCTSTR Set(LPCTSTR key, LPCTSTR pData);
		bool Remove(LPCTSTR key);
		void RemoveAll();
		int GetSize() const;
		LPCTSTR GetAt(int iIndex) const;
		LPCTSTR operator[] (int nIndex) const;

	protected:
		STRINGMAPITEM** m_aT;
		int m_nBuckets;
		int m_nCount;
	};

	class CStringTable
	{
	public:
		CStringTable();
		CStringTable(STRINGorID xml, LPCTSTR type, LPCTSTR lpszLang);
		~CStringTable(void);
		bool Load(STRINGorID xml, LPCTSTR type, LPCTSTR lpszLang);
		void Parse();
		LPCTSTR GetString(LPCTSTR lpszID);
		LPCTSTR FormatString(LPCTSTR lpszSource);
		static LPCTSTR FormatString(CStringTable* pStringTable, LPCTSTR lpszSource);
	private:
		CMarkup m_xml;
		LPCTSTR m_pstrtype;
		CDuiStringMap m_mapStringHash;
		CDuiString m_strLan;
	};

}