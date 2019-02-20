#include "duipub.h"
#include "UIStringTable.h"

namespace DuiLib{

	CDuiStringTable::CDuiStringTable():m_mapStringHash(16), m_pstrtype(NULL)
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
		Clear();
		CDuiXmlNode root = m_xml.GetRoot();
		if (!root)
		{
			return;
		}
		if (!m_strLan.GetLength())
		{
			CDuiXmlAttr attr = root.attribute(_T("default"));
			LPCTSTR lpszTmp = attr.as_string();
			if (lpszTmp && _tcslen(lpszTmp))
			{
				m_strLan = lpszTmp;
			}
		}
		if (m_strLan.GetLength())
		{
			CDuiXmlNode node = root.child(m_strLan);
			if (node)
			{
				root = node;
			}
		}

		for( CDuiXmlNode node = root.first_child(); node; node = node.next_sibling() ) 
		{
			CDuiString strValue = node.text().as_string();
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
			CDuiString* pOld = (CDuiString*)m_mapStringHash.Find(node.name());
			if (pOld)
			{
				CDuiString sDbg(_T("Warning: Same string ID <"));
				sDbg += strValue;
				sDbg += _T(">\r\n");
				OutputDebugString(sDbg);
				delete pOld;
			}
			m_mapStringHash.Set(node.name(), pString);

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
		CDuiBuffer buf;
		if (!DuiReadResourceFileData(lpszFile, buf))
		{
			return false;
		}
		bool ret = m_xml.LoadFromMem(buf, buf.GetSize());
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