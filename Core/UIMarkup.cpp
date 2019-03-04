#include "duipub.h"

#ifndef TRACE
#define TRACE
#endif


namespace DuiLib {

///////////////////////////////////////////////////////////////////////////////////////
//
//
//

	CMarkup::CMarkup(LPCTSTR pstrXML)
		:m_pstrXML(NULL)
	{
		if( pstrXML != NULL ) Load(pstrXML);
	}

CMarkup::~CMarkup()
{
    Release();
}

bool CMarkup::IsValid() const
{
	return !!GetRoot();
}

bool CMarkup::Load(LPCTSTR pstrXML)
{
    Release();
    SIZE_T cchLen = _tcslen(pstrXML) + 1;
    m_pstrXML = static_cast<LPTSTR>(malloc(cchLen * sizeof(TCHAR)));
    ::CopyMemory(m_pstrXML, pstrXML, cchLen * sizeof(TCHAR));
    bool bRes = _Parse();
    if( !bRes ) Release();
    return bRes;
}

bool CMarkup::LoadFromMem(BYTE* pByte, DWORD dwSize, int encoding)
{
#ifdef _UNICODE
    if (encoding == XMLFILE_ENCODING_UTF8)
    {
        if( dwSize >= 3 && pByte[0] == 0xEF && pByte[1] == 0xBB && pByte[2] == 0xBF ) 
        {
            pByte += 3; dwSize -= 3;
        }
        DWORD nWide = ::MultiByteToWideChar( CP_UTF8, 0, (LPCSTR)pByte, dwSize, NULL, 0 );

        m_pstrXML = static_cast<LPTSTR>(malloc((nWide + 1)*sizeof(TCHAR)));
        ::MultiByteToWideChar( CP_UTF8, 0, (LPCSTR)pByte, dwSize, m_pstrXML, nWide );
        m_pstrXML[nWide] = _T('\0');
    }
    else if (encoding == XMLFILE_ENCODING_ASNI)
    {
        DWORD nWide = ::MultiByteToWideChar( CP_ACP, 0, (LPCSTR)pByte, dwSize, NULL, 0 );

        m_pstrXML = static_cast<LPTSTR>(malloc((nWide + 1)*sizeof(TCHAR)));
        ::MultiByteToWideChar( CP_ACP, 0, (LPCSTR)pByte, dwSize, m_pstrXML, nWide );
        m_pstrXML[nWide] = _T('\0');
    }
    else
    {
        if ( dwSize >= 2 && ( ( pByte[0] == 0xFE && pByte[1] == 0xFF ) || ( pByte[0] == 0xFF && pByte[1] == 0xFE ) ) )
        {
            dwSize = dwSize / 2 - 1;

            if ( pByte[0] == 0xFE && pByte[1] == 0xFF )
            {
                pByte += 2;

                for ( DWORD nSwap = 0 ; nSwap < dwSize ; nSwap ++ )
                {
                    register CHAR nTemp = pByte[ ( nSwap << 1 ) + 0 ];
                    pByte[ ( nSwap << 1 ) + 0 ] = pByte[ ( nSwap << 1 ) + 1 ];
                    pByte[ ( nSwap << 1 ) + 1 ] = nTemp;
                }
            }
            else
            {
                pByte += 2;
            }

            m_pstrXML = static_cast<LPTSTR>(malloc((dwSize + 1)*sizeof(TCHAR)));
            ::CopyMemory( m_pstrXML, pByte, dwSize * sizeof(TCHAR) );
            m_pstrXML[dwSize] = _T('\0');

            pByte -= 2;
        }
    }
#else // !_UNICODE
    if (encoding == XMLFILE_ENCODING_UTF8)
    {
        if( dwSize >= 3 && pByte[0] == 0xEF && pByte[1] == 0xBB && pByte[2] == 0xBF ) 
        {
            pByte += 3; dwSize -= 3;
        }
        DWORD nWide = ::MultiByteToWideChar( CP_UTF8, 0, (LPCSTR)pByte, dwSize, NULL, 0 );

        LPWSTR w_str = static_cast<LPWSTR>(malloc((nWide + 1)*sizeof(WCHAR)));
        ::MultiByteToWideChar( CP_UTF8, 0, (LPCSTR)pByte, dwSize, w_str, nWide );
        w_str[nWide] = L'\0';

        DWORD wide = ::WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)w_str, nWide, NULL, 0, NULL, NULL);

        m_pstrXML = static_cast<LPTSTR>(malloc((wide + 1)*sizeof(TCHAR)));
        ::WideCharToMultiByte( CP_ACP, 0, (LPCWSTR)w_str, nWide, m_pstrXML, wide, NULL, NULL);
        m_pstrXML[wide] = _T('\0');

        free(w_str);
    }
    else if (encoding == XMLFILE_ENCODING_UNICODE)
    {
        if ( dwSize >= 2 && ( ( pByte[0] == 0xFE && pByte[1] == 0xFF ) || ( pByte[0] == 0xFF && pByte[1] == 0xFE ) ) )
        {
            dwSize = dwSize / 2 - 1;

            if ( pByte[0] == 0xFE && pByte[1] == 0xFF )
            {
                pByte += 2;

                for ( DWORD nSwap = 0 ; nSwap < dwSize ; nSwap ++ )
                {
                    register CHAR nTemp = pByte[ ( nSwap << 1 ) + 0 ];
                    pByte[ ( nSwap << 1 ) + 0 ] = pByte[ ( nSwap << 1 ) + 1 ];
                    pByte[ ( nSwap << 1 ) + 1 ] = nTemp;
                }
            }
            else
            {
                pByte += 2;
            }

            DWORD nWide = ::WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)pByte, dwSize, NULL, 0, NULL, NULL);
            m_pstrXML = static_cast<LPTSTR>(malloc((nWide + 1)*sizeof(TCHAR)));
            ::WideCharToMultiByte( CP_ACP, 0, (LPCWSTR)pByte, dwSize, m_pstrXML, nWide, NULL, NULL);
            m_pstrXML[nWide] = _T('\0');

            pByte -= 2;
        }
    }
    else
    {
        m_pstrXML = static_cast<LPTSTR>(malloc((dwSize + 1)*sizeof(TCHAR)));
        ::CopyMemory( m_pstrXML, pByte, dwSize * sizeof(TCHAR) );
        m_pstrXML[dwSize] = _T('\0');
    }
#endif // _UNICODE

    bool bRes = _Parse();
    if( !bRes ) Release();
    return bRes;
}

bool CMarkup::LoadFromFile(LPCTSTR pstrFilename, int encoding)
{
	Release();
	CDuiBuffer buf;
	if (!DuiReadResourceFileData(pstrFilename, buf))
	{
		return false;
	}

	bool ret = LoadFromMem(buf, buf.GetSize(), encoding);
	return ret;
}

void CMarkup::Release()
{
	if( m_pstrXML != NULL ) free(m_pstrXML);
	m_pstrXML=NULL;
	m_parser.reset();
}

CDuiXmlNode CMarkup::GetRoot() const
{
	return m_parser.root().first_child();
}

bool CMarkup::_Parse()
{
	if (!m_pstrXML)
		return false;
	pugi::xml_parse_result result = m_parser.load_string(m_pstrXML);

	m_error = result.description();
	return result.status == pugi::status_ok;
}

bool CMarkup::_Failed(LPCTSTR pstrError, LPCTSTR pstrLocation)
{
    // Register last error
    TRACE(_T("XML Error: %s"), pstrError);
    if( pstrLocation != NULL ) TRACE(pstrLocation);
	m_error = pstrError;
//     _tcsncpy(m_szErrorMsg, pstrError, (sizeof(m_szErrorMsg) / sizeof(m_szErrorMsg[0])) - 1);
//     _tcsncpy(m_szErrorXML, pstrLocation != NULL ? pstrLocation : _T(""), lengthof(m_szErrorXML) - 1);
// 	OutputDebugString(pstrError);
// 	OutputDebugString(pstrLocation);
    return false; // Always return 'false'
}

} // namespace DuiLib
