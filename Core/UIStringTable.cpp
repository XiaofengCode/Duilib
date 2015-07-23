#include "stdafx.h"
#include "UIStringTable.h"

namespace DuiLib{

	CDuiStringTable::CDuiStringTable():m_mapStringHash(16)
	{
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
	void CDuiStringTable::Parse()
	{
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
			m_mapStringHash.Set(node.GetName(), pString);
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
		return *pString;
	}

	LPCTSTR CDuiStringTable::FormatString( LPCTSTR lpszSource )
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
			hFile = ::CreateFile(CPaintManagerUI::GetInstancePath() + lpszFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if( hFile == INVALID_HANDLE_VALUE )
			{
				hFile = ::CreateFile(CPaintManagerUI::GetResourcePath() + lpszFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				if( hFile == INVALID_HANDLE_VALUE )
				{
					return _Failed(_T("LoadFromFile Error opening file"));
				}
			}
		}
		DWORD dwSize = ::GetFileSize(hFile, NULL);
		if( dwSize == 0 ) return _Failed(_T("LoadFromFile File is empty"));
		if ( dwSize > 4096*1024 ) return _Failed(_T("LoadFromFile File too large"));

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
		OutputDebugString(pstrLocation);
		return false;
	}

	bool CDuiStringTable::IsValid()
	{
		return m_xml.IsValid();
	}

}