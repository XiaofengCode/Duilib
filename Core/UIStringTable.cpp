#include "duipub.h"
#include "UIStringTable.h"

namespace DuiLib{

	CDuiStringTable::CDuiStringTable(CPaintManagerUI* pManager):m_mapStringHash(16), m_pstrtype(NULL), m_pManager(pManager), m_xml(pManager)
	{
	}


	CDuiStringTable::~CDuiStringTable()
	{
		Clear();
	}

	bool CDuiStringTable::Load( STRINGorID xml, LPCTSTR type, LPCTSTR lpszLang )
	{
		if (lpszLang && _tcslen(lpszLang))
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
			HRSRC hResource = ::FindResource(m_pManager->GetResourceDll(), xml.m_lpstr, type);
			if( hResource == NULL )
			{
				return false;
			}
			HGLOBAL hGlobal = ::LoadResource(m_pManager->GetResourceDll(), hResource);
			if( hGlobal == NULL ) 
			{
				FreeResource(hResource);
				{
					return false;
				}
			}

			if( !m_xml.LoadFromMem((BYTE*)::LockResource(hGlobal), ::SizeofResource(m_pManager->GetResourceDll(), hResource) ))
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
	void CDuiStringTable::Parse()
	{
		Clear();
		CMarkupNode root = m_xml.GetRoot();
		if (!root.IsValid())
		{
			return;
		}
		if (!m_strLan.GetLength())
		{
			LPCTSTR lpszTmp = root.GetAttributeValue(_T("default"));
			if (lpszTmp && _tcslen(lpszTmp))
			{
				m_strLan = lpszTmp;
			}
		}
		if (m_strLan.GetLength())
		{
			CMarkupNode node = root.GetChild(m_strLan);
			if (node.IsValid())
			{
				root = node;
			}
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

			CDuiString* pString = new CDuiString(strValue);
			CDuiString* pOld = (CDuiString*)m_mapStringHash.Find(node.GetName());
			if (pOld)
			{
				CDuiString sDbg(_T("Warning: Same string ID <"));
				sDbg += strValue;
				sDbg += _T(">\r\n");
				OutputDebugString(sDbg);
				delete pOld;
			}
			m_mapStringHash.Set(node.GetName(), pString);

			delete[]szBuf;
			szBuf = NULL;
		}
	}

	LPCTSTR CDuiStringTable::GetString( LPCTSTR lpszID )
	{
		CDuiString* pString = NULL;
		pString = (CDuiString*)m_mapStringHash.Find(lpszID);
		if (!pString)
		{
			return NULL;
		}
		
		int nPos = pString->Find(STRINGID_ID_2);
		while (nPos != -1)
		{
			int nPos2 = pString->Find(_T("#"), nPos + 3);
			if (nPos2 != -1)
			{
				LPCTSTR lpsz = GetString(pString->Mid(nPos + 1, nPos2 - nPos - 1));
				if (lpsz)
				{
					pString->Replace(pString->Mid(nPos, nPos2 - nPos + 1), lpsz);
				}
			}
			nPos = pString->Find(STRINGID_ID_2, nPos + 3);			
		}

		nPos = pString->Find(STRINGID_ID);
		while (nPos != -1)
		{
			int nPos2 = pString->Find(_T("@"), nPos + 3);
			if (nPos2 != -1)
			{
				LPCTSTR lpsz = GetString(pString->Mid(nPos + 1, nPos2 - nPos - 1));
				if (lpsz)
				{
					pString->Replace(pString->Mid(nPos, nPos2 - nPos + 1), lpsz);
				}
			}
			nPos = pString->Find(STRINGID_ID_2, nPos + 3);			
		}
		return *pString;
	}

	LPCTSTR CDuiStringTable::FormatString( LPCTSTR lpszSource )
	{
		if (_tcsncmp(lpszSource, STRINGID_ID, _tcslen(STRINGID_ID)) == 0 ||
			_tcsncmp(lpszSource, STRINGID_ID_2, _tcslen(STRINGID_ID_2)) == 0)
		{
			LPCTSTR lpszString = GetString(lpszSource + 1);
			if (lpszString)
			{
				return lpszString;
			}
		}
		return lpszSource;
	}

	LPCTSTR CDuiStringTable::FormatString( CPaintManagerUI* pManager, LPCTSTR lpszSource )
	{
		if (!pManager)
		{
			return lpszSource;
		}
		return pManager->GetStringTable().FormatString(lpszSource);
	}

	bool CDuiStringTable::LoadFromFile( LPCTSTR lpszFile, LPCTSTR lpszLang )
	{
		if (lpszLang)
		{
			m_strLan = lpszLang;
		}
		else
		{
			m_strLan.Empty();
		}
		HANDLE hFile = ::CreateFile(lpszFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if( hFile == INVALID_HANDLE_VALUE )
		{
			hFile = ::CreateFile(m_pManager->GetInstancePath() + lpszFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if( hFile == INVALID_HANDLE_VALUE )
			{
				hFile = ::CreateFile(m_pManager->GetResourcePath() + lpszFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				if( hFile == INVALID_HANDLE_VALUE )
				{
					return Load(lpszFile, 0, lpszLang);
				}
			}
		}
		DWORD dwSize = ::GetFileSize(hFile, NULL);
		if( dwSize == 0 )
		{
			CloseHandle(hFile);
			return _Failed(_T("LoadFromFile File is empty"));
		}
		if ( dwSize > 4096*1024 )
		{
			CloseHandle(hFile);
			return _Failed(_T("LoadFromFile File too large"));
		}

		DWORD dwRead = 0;
		BYTE* pByte = new BYTE[ dwSize ];
		::ReadFile( hFile, pByte, dwSize, &dwRead, NULL );
		::CloseHandle( hFile );

		if( dwRead != dwSize ) 
		{
			delete[] pByte;
			return _Failed(_T("LoadFromFile Could not read file"));
		}
		bool ret = m_xml.LoadFromMem(pByte, dwSize);
		delete[] pByte;
		if (!ret)
		{
			return _Failed(_T("LoadFromFile LoadFromMem failed"));
		}
		Parse();
		return true;
	}

	bool CDuiStringTable::_Failed( LPCTSTR pstrError, LPCTSTR pstrLocation /*= NULL*/ )
	{
		OutputDebugString(pstrError);
		return false;
	}

	bool CDuiStringTable::IsValid()
	{
		return m_xml.IsValid();
	}

	void CDuiStringTable::Clear()
	{
		for (int i = 0; i < m_mapStringHash.GetSize(); i++)
		{
			LPCTSTR lpszKey = m_mapStringHash.GetAt(i);
			if (!lpszKey)
			{
				continue;
			}
			CDuiString* pString = (CDuiString*)m_mapStringHash.Find(lpszKey);
			if (pString)
			{
				delete pString;
			}
		}
		m_mapStringHash.RemoveAll();
	}

}