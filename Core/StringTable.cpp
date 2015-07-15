#include "stdafx.h"
#include "StringTable.h"

namespace DuiLib{


	static UINT HashKey(LPCTSTR Key)
	{
		UINT i = 0;
		SIZE_T len = _tcslen(Key);
		while( len-- > 0 ) i = (i << 5) + i + Key[len];
		return i;
	}

	static UINT HashKey(const CDuiString& Key)
	{
		return HashKey((LPCTSTR)Key);
	};

	CDuiStringMap::CDuiStringMap(int nSize) : m_nCount(0)
	{
		if( nSize < 16 ) nSize = 16;
		m_nBuckets = nSize;
		m_aT = new STRINGMAPITEM*[nSize];
		memset(m_aT, 0, nSize * sizeof(STRINGMAPITEM*));
	}

	CDuiStringMap::~CDuiStringMap()
	{
		if( m_aT ) {
			int len = m_nBuckets;
			while( len-- ) {
				STRINGMAPITEM* pItem = m_aT[len];
				while( pItem ) {
					STRINGMAPITEM* pKill = pItem;
					pItem = pItem->pNext;
					delete pKill;
				}
			}
			delete [] m_aT;
			m_aT = NULL;
		}
	}

	void CDuiStringMap::RemoveAll()
	{
		this->Resize(m_nBuckets);
	}

	void CDuiStringMap::Resize(int nSize)
	{
		if( m_aT ) 
		{
			int len = m_nBuckets;
			while( len-- ) 
			{
				STRINGMAPITEM* pItem = m_aT[len];
				while( pItem ) 
				{
					STRINGMAPITEM* pKill = pItem;
					pItem = pItem->pNext;
					delete pKill;
				}
			}
			delete [] m_aT;
			m_aT = NULL;
		}

		if( nSize < 0 ) nSize = 0;
		if( nSize > 0 ) 
		{
			m_aT = new STRINGMAPITEM*[nSize];
			memset(m_aT, 0, nSize * sizeof(STRINGMAPITEM*));
		} 
		m_nBuckets = nSize;
		m_nCount = 0;
	}

	LPCTSTR CDuiStringMap::Find(LPCTSTR key, bool optimize) const
	{
		if( m_nBuckets == 0 || GetSize() == 0 ) return NULL;

		UINT slot = HashKey(key) % m_nBuckets;
		for( STRINGMAPITEM* pItem = m_aT[slot]; pItem; pItem = pItem->pNext )
		{
			if( pItem->Key == key ) 
			{
				if (optimize && pItem != m_aT[slot]) 
				{
					if (pItem->pNext) 
					{
						pItem->pNext->pPrev = pItem->pPrev;
					}
					pItem->pPrev->pNext = pItem->pNext;
					pItem->pPrev = NULL;
					pItem->pNext = m_aT[slot];
					pItem->pNext->pPrev = pItem;
					//将item移动至链条头部
					m_aT[slot] = pItem;
				}
				return pItem->Value;
			}        
		}

		return NULL;
	}

	bool CDuiStringMap::Insert(LPCTSTR key, LPCTSTR value)
	{
		if( m_nBuckets == 0 ) return false;
		if( Find(key) ) return false;

		// Add first in bucket
		UINT slot = HashKey(key) % m_nBuckets;
		STRINGMAPITEM* pItem = new STRINGMAPITEM;
		pItem->Key = key;
		pItem->Value = value;
		pItem->pPrev = NULL;
		pItem->pNext = m_aT[slot];
		if (pItem->pNext)
		{
			pItem->pNext->pPrev = pItem;
		}
		m_aT[slot] = pItem;
		m_nCount++;
		return true;
	}

	LPCTSTR CDuiStringMap::Set(LPCTSTR key, LPCTSTR pData)
	{
		if( m_nBuckets == 0 ) 
		{
			return pData;
		}

		if (GetSize()>0) 
		{
			UINT slot = HashKey(key) % m_nBuckets;
			// Modify existing item
			for( STRINGMAPITEM* pItem = m_aT[slot]; pItem; pItem = pItem->pNext ) 
			{
				if( pItem->Key == key ) 
				{
					LPCTSTR pOldData = pItem->Value;
					pItem->Value = pData;
					return pOldData;
				}
			}
		}

		Insert(key, pData);
		return NULL;
	}

	bool CDuiStringMap::Remove(LPCTSTR key)
	{
		if( m_nBuckets == 0 || GetSize() == 0 ) 
		{
			return false;
		}

		UINT slot = HashKey(key) % m_nBuckets;
		STRINGMAPITEM** ppItem = &m_aT[slot];
		while( *ppItem ) 
		{
			if( (*ppItem)->Key == key ) 
			{
				STRINGMAPITEM* pKill = *ppItem;
				*ppItem = (*ppItem)->pNext;
				if (*ppItem)
				{
					(*ppItem)->pPrev = pKill->pPrev;
				}
				delete pKill;
				m_nCount--;
				return true;
			}
			ppItem = &((*ppItem)->pNext);
		}

		return false;
	}

	int CDuiStringMap::GetSize() const
	{
#if 0//def _DEBUG
		int nCount = 0;
		int len = m_nBuckets;
		while( len-- )
		{
			for( const STRINGMAPITEM* pItem = m_aT[len]; pItem; pItem = pItem->pNext ) 
			{
				nCount++;
			}
		}
		ASSERT(m_nCount==nCount);
#endif
		return m_nCount;
	}

	LPCTSTR CDuiStringMap::GetAt(int iIndex) const
	{
		if( m_nBuckets == 0 || GetSize() == 0 )
		{
			return false;
		}

		int pos = 0;
		int len = m_nBuckets;
		while( len-- ) 
		{
			for( STRINGMAPITEM* pItem = m_aT[len]; pItem; pItem = pItem->pNext ) 
			{
				if( pos++ == iIndex ) 
				{
					return pItem->Key.GetData();
				}
			}
		}

		return NULL;
	}

	LPCTSTR CDuiStringMap::operator[] (int nIndex) const
	{
		return GetAt(nIndex);
	}

	CStringTable::CStringTable(STRINGorID xml, LPCTSTR type, LPCTSTR lpszLan)
	{
		if (lpszLan)
		{
			m_strLan = lpszLan;
		}
		Load(xml, type, lpszLan);
	}

	CStringTable::CStringTable()
	{

	}


	CStringTable::~CStringTable(void)
	{
	}

	bool CStringTable::Load( STRINGorID xml, LPCTSTR type, LPCTSTR lpszLang )
	{
		if (lpszLang)
		{
			m_strLan = lpszLang;
		}
		else
		{
			m_strLan = _T("");
		}
		if( HIWORD(xml.m_lpstr) != NULL ) 
		{
			if( *(xml.m_lpstr) == _T('<') ) 
			{
				if( !m_xml.Load(xml.m_lpstr) ) 
				{
					return false;
				}
			}
			else 
			{
				if( !m_xml.LoadFromFile(xml.m_lpstr) ) 
				{
					return false;
				}
			}
		}
		else 
		{
			HRSRC hResource = ::FindResource(CPaintManagerUI::GetResourceDll(), xml.m_lpstr, type);
			if( hResource == NULL )
			{
				return false;
			}
			HGLOBAL hGlobal = ::LoadResource(CPaintManagerUI::GetResourceDll(), hResource);
			if( hGlobal == NULL ) 
			{
				FreeResource(hResource);
				{
					return false;
				}
			}

			if( !m_xml.LoadFromMem((BYTE*)::LockResource(hGlobal), ::SizeofResource(CPaintManagerUI::GetResourceDll(), hResource) ))
			{
				return false;
			}
			::FreeResource(hResource);
			m_pstrtype = type;
		}
		Parse();
		return true;
	}

	//转义字符
	struct STRING_PAIR 
	{
		CDuiString str1;
		CDuiString str2;
	};
	static STRING_PAIR StaticEntityNames[] = {
		{_T("\\\\"), _T("\\")},
		{_T("\\n"), _T("\n")},
		{_T("\\t"), _T("\t")},
		{_T("\\"), _T("")},
	};
	void CStringTable::Parse()
	{
		CMarkupNode root = m_xml.GetRoot();
		if (!root.IsValid())
		{
			return;
		}
		if (!m_strLan.GetLength())
		{
			m_strLan = root.GetAttributeValue(_T("default"));
		}
		if (m_strLan.GetLength())
		{
			root = root.GetChild(m_strLan);
		}
		for( CMarkupNode node = root.GetChild() ; node.IsValid(); node = node.GetSibling() ) 
		{
			CDuiString strValue = node.GetValue();
			//转义字符
			int nLen = strValue.GetLength();
			TCHAR* szBuf = new TCHAR[nLen + 1];
			_tcscpy(szBuf, strValue.GetData());
			strValue.Empty();
			for (int i = 0;i < nLen; i++)
			{
				TCHAR cLetter = szBuf[i];
				if (cLetter == '\\')
				{
					++i;
					switch (szBuf[i])
					{
					case 'n':
						cLetter = '\n';
						break;
					case '\\':
						cLetter = '\\';
						break;
					case 'r':
						cLetter = '\r';
						break;
					case 't':
						cLetter = _T('\t');
						break;
					default:
						--i;
						break;
					}
				}
				else if (cLetter == '&')
				{
					if (szBuf[i + 1] == 'l' && szBuf[i + 1] == 't' && szBuf[i + 2] == ';')
					{
						i += 3;
						cLetter = '<';
					}
					else if (szBuf[i + 1] == 'g' && szBuf[i + 1] == 't' && szBuf[i + 2] == ';')
					{
						i += 3;
						cLetter = '>';
					}
					else if (szBuf[i + 1] == 'q' && 
						szBuf[i + 1] == 'u' && 
						szBuf[i + 2] == 'o' && 
						szBuf[i + 3] == 't' && 
						szBuf[i + 4] == ';')
					{
						i += 5;
						cLetter = '\"';
					}
				}
				strValue += cLetter;
			}
// 			strValue.Replace(_T("\\n"), _T("\n"));
// 			strValue.Replace(_T("\\t"), _T("\t"));
// 			strValue.Replace(_T("\\r"), _T("\r"));
// 			strValue.Replace(_T("&lt;"), _T("<"));
// 			strValue.Replace(_T("&gt;"), _T(">"));
// 			strValue.Replace(_T("&quot;"), _T("\""));
// 			strValue.Replace(_T("\\\\"), _T("\\"));
			m_mapStringHash.Set(node.GetName(), strValue);
		}
	}

	LPCTSTR CStringTable::GetString( LPCTSTR lpszID )
	{
		return m_mapStringHash.Find(lpszID);
	}

	LPCTSTR CStringTable::FormatString( LPCTSTR lpszSource )
	{
		if (_tcsncmp(lpszSource, STRINGID_ID, _tcslen(STRINGID_ID)) == 0)
		{
			LPCTSTR lpszString = GetString(lpszSource + 1);
			if (lpszString)
			{
				return lpszString;
			}
		}
		return lpszSource;
	}

	LPCTSTR CStringTable::FormatString( CStringTable* pStringTable, LPCTSTR lpszSource )
	{
		if (!pStringTable)
		{
			return lpszSource;
		}
		return pStringTable->FormatString(lpszSource);
	}

}