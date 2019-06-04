#include "duipub.h"
#include "FileHelper.h"
#include <io.h>
#include <winhttp.h>
#pragma comment(lib,"Winhttp.lib")

namespace DuiLib {
	bool _Failed(LPCTSTR pstrError, LPCTSTR pstrLocation /*= NULL*/)
	{
		OutputDebugString(pstrError);
		OutputDebugString(_T("\r\n"));;
		return false;
	}

	CDuiString DuiGetAbsFilePath(LPCTSTR pstrFilename)
	{
		CDuiString sPath = CPaintManagerUI::GetResourcePath() + pstrFilename;
		if (_taccess(sPath, 0) == 0)
		{
			return sPath;
		}
		sPath = pstrFilename;
		if (_taccess(sPath, 0) == 0)
		{
			return sPath;
		}
		sPath = CPaintManagerUI::GetInstancePath() + pstrFilename;
		if (_taccess(sPath, 0) == 0)
		{
			return sPath;
		}
		return pstrFilename;
	}

// 	std::string DuiGetAbsFilePath(LPCSTR pstrFilename)
// 	{
// 		CDuiString sPath = DuiGetAbsFilePath(CDuiString(pstrFilename));
// 		return DUI_T2A(sPath);
// 	}

	BOOL HttpRequest(LPCTSTR lpszServerName, INTERNET_PORT port, bool bSSL, LPCTSTR lpszObjectName, LPCTSTR lpszMethod, LPVOID lpData, DWORD dwDataSize, CDuiBuffer& bufRes)
	{
		HINTERNET hOpen = 0;
		HINTERNET hConnect = 0;
		HINTERNET hRequest = 0;
		BOOL bResult = FALSE;

		do {
			hOpen = WinHttpOpen(_T("DownloadFile"), WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
				WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
			if (!hOpen) {
				wprintf(L"WinHttpOpen failed (0x%.8X)\n", GetLastError());
				break;
			}
			hConnect = WinHttpConnect(hOpen, lpszServerName, port, 0);
			if (!hConnect) {
				DWORD dwErr = GetLastError();
				wprintf(L"WinHttpConnect failed (0x%.8X)\n", GetLastError());
				break;
			}
			hRequest = WinHttpOpenRequest(hConnect, lpszMethod, lpszObjectName,
				NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, bSSL ? WINHTTP_FLAG_SECURE : 0);
			if (!hRequest)
			{
				wprintf(L"WinHttpOpenRequest failed (0x%.8X)\n", GetLastError());
				break;
			}
			TCHAR headerContentType[] = _T("Content-Type: */*";);
			TCHAR headerContentLength[64];
			_stprintf_s(headerContentLength, 64, _T("Content-Length: %d\r\n\r\n"), dwDataSize);
			WinHttpAddRequestHeaders(hRequest, headerContentType, -1, WINHTTP_ADDREQ_FLAG_ADD | WINHTTP_ADDREQ_FLAG_REPLACE);
			WinHttpAddRequestHeaders(hRequest, headerContentLength, -1, WINHTTP_ADDREQ_FLAG_ADD | WINHTTP_ADDREQ_FLAG_REPLACE);
			if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, lpData, dwDataSize, 0, 0))
			{
				wprintf(L"WinHttpSendRequest failed (0x%.8X)\n", GetLastError());
				break;
			}
			if (!WinHttpReceiveResponse(hRequest, 0))
			{
				wprintf(L"WinHttpReceiveResponse failed (0x%.8X)\n", GetLastError());
				break;
			}
			TCHAR szContentLength[32] = { 0 };
			DWORD cch = 64;
			DWORD dwHeaderIndex = WINHTTP_NO_HEADER_INDEX;
			BOOL haveContentLength = WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_CONTENT_LENGTH, NULL,
				&szContentLength, &cch, &dwHeaderIndex);
			DWORD dwContentLength;
			if (haveContentLength) dwContentLength = _wtoi(szContentLength);

			BYTE byBuffer[4097];
			DWORD dwSize;
			std::vector<CDuiBuffer*> arResBuf;
			while (WinHttpQueryDataAvailable(hRequest, &dwSize) && dwSize) {
				if (dwSize > 4096) dwSize = 4096;

				WinHttpReadData(hRequest, byBuffer, dwSize, &dwSize);
				byBuffer[dwSize] = 0;
				CDuiBuffer* pBuf = new CDuiBuffer(dwSize);
				memcpy(*pBuf, byBuffer, dwSize);
				arResBuf.push_back(pBuf);
			}
			dwSize = 0;
			for (size_t i = 0; i < arResBuf.size(); i++)
			{
				dwSize += arResBuf[i]->GetSize();
			}
			bufRes.Resize(dwSize);
			dwSize = 0;
			for (size_t i = 0; i < arResBuf.size(); i++)
			{
				memcpy((LPBYTE)bufRes + dwSize, *arResBuf[i], arResBuf[i]->GetSize());
				dwSize += arResBuf[i]->GetSize();
				delete arResBuf[i];
			}			
			bResult = TRUE;
		} while (FALSE);

		if (hRequest) WinHttpCloseHandle(hRequest);
		if (hConnect) WinHttpCloseHandle(hConnect);
		if (hOpen) WinHttpCloseHandle(hOpen);

		return bResult;
	}

	BOOL DuiReadFileData(LPCTSTR pstrFilename, CDuiBuffer& buffer)
	{
		if (_tcsnicmp(pstrFilename, _T("http://"), _tcslen(_T("http://"))) == 0
			|| _tcsnicmp(pstrFilename, _T("https://"), _tcslen(_T("https://"))) == 0)
		{
			INTERNET_PORT port = INTERNET_DEFAULT_HTTP_PORT;
			CDuiString strServer;
			CDuiString strObject;
			bool bSSL = false;
			LPTSTR lpszUrl = _tcsdup(pstrFilename);
			LPTSTR p = lpszUrl;
			p += 4;		//	"http"
			if (*p == 's')
			{
				bSSL = true;
				port = INTERNET_DEFAULT_HTTPS_PORT;
				p++;
			}
			p += 3;		//	"://"
			LPTSTR pPort = _tcschr(p, ':');
			if (pPort)
			{
				*pPort = 0;
				strServer = p;
				p = pPort;
			}
			LPTSTR pSubPath = _tcschr(p, '/');
			*pSubPath = 0;
			if (pPort)
			{
				//Port
				port = (INTERNET_PORT)_ttoi(p);
			}
			else
			{
				strServer = p;
			}
			*pSubPath = '/';
			strObject = pSubPath;
			return HttpRequest(strServer, port, bSSL, strObject, _T("GET"), NULL, 0, buffer);
		}
		CDuiString strFile = DuiGetAbsFilePath(pstrFilename);
		HANDLE hFile = CreateFile(strFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if( hFile == INVALID_HANDLE_VALUE )
		{
			CDuiString sErr(_T("Error opening file:"));
			sErr += strFile;
			return _Failed(sErr);
		}
		DWORD dwSize = ::GetFileSize(hFile, NULL);
		if( dwSize == 0 )
		{
			CloseHandle(hFile);
			return _Failed(_T("File is empty"));
		}
		if ( dwSize > 10*4096*1024 )
		{
			CloseHandle(hFile);
			return _Failed(_T("File too large"));
		}

		DWORD dwRead = 0;
		//BYTE* pByte = new BYTE[ dwSize ];
		buffer.Resize(dwSize);
		::ReadFile( hFile, buffer, dwSize, &dwRead, NULL );
		::CloseHandle( hFile );

		if( dwRead != dwSize )
		{
			buffer.Release();
			return _Failed(_T("Could not read file"));
		}
		return true;
	}

	BOOL DuiReadResourceFileData(LPCTSTR pstrFilename, CDuiBuffer& buffer)
	{
		if( CPaintManagerUI::GetResourceZip().IsEmpty() )
		{
			return DuiReadFileData(pstrFilename, buffer);
		}
		else
		{
			HZIP hz = NULL;
			if( CPaintManagerUI::IsCachedResourceZip() )
				hz = (HZIP)CPaintManagerUI::GetResourceZipHandle();
			if (hz == NULL)
			{
				hz = OpenZip((void*)(LPCTSTR)(DuiGetAbsFilePath(CPaintManagerUI::GetResourceZip())), 0, 2);
				if (hz == NULL)
				{
					return DuiReadFileData(pstrFilename, buffer);
					//return _Failed(_T("Error opening zip file"));
				}
			}

			ZIPENTRY ze; 
			int i; 
			if( FindZipItem(hz, pstrFilename, true, &i, &ze) != 0 )
			{
				return DuiReadFileData(pstrFilename, buffer);
				//return _Failed(_T("Could not find ziped file"));
			}
			DWORD dwSize = ze.unc_size;
			if( dwSize == 0 )
				return _Failed(_T("File is empty"));
			if ( dwSize > 10 * 4096*1024 )
				return _Failed(_T("File too large"));
			buffer.Resize(dwSize);
			int res = UnzipItem(hz, i, buffer, dwSize, 3);
			if( res != 0x00000000 && res != 0x00000600)
			{
				buffer.Release();
				if( !CPaintManagerUI::IsCachedResourceZip() )
					CloseZip(hz);
				return _Failed(_T("Could not unzip file"));
			}
			if( !CPaintManagerUI::IsCachedResourceZip() )
				CloseZip(hz);
			return true;
		}
	}

	CDuiBuffer::CDuiBuffer(DWORD dwSize)
	{
		m_lpMem = NULL;
		m_dwSize = dwSize;
		if (m_dwSize)
		{
			m_lpMem = malloc(dwSize);
			if (m_lpMem)
			{
				memset(m_lpMem, 0, dwSize);
			}
		}
	}

	CDuiBuffer::~CDuiBuffer()
	{
		Release();
	}

	BOOL CDuiBuffer::Resize(DWORD dwNewSize)
	{
		if (m_lpMem)
		{
			LPVOID lpNewMem = realloc(m_lpMem, dwNewSize);
			if (!lpNewMem)return FALSE;
			memcpy(lpNewMem, m_lpMem, min(m_dwSize, dwNewSize));
			m_dwSize = dwNewSize;
			m_lpMem = lpNewMem;
		}
		else
		{
			m_lpMem = malloc(dwNewSize);
			if (!m_lpMem)return FALSE;
			memset(m_lpMem, 0, dwNewSize);
			m_dwSize = dwNewSize;
		}
		return TRUE;
	}

	void CDuiBuffer::Release()
	{
		m_dwSize = 0;
		if (m_lpMem)
		{
			free(m_lpMem);
			m_lpMem = NULL;
		}
	}

	bool CDuiBuffer::SaveAs(LPCTSTR lpszFileName)
	{
		HANDLE hFile = CreateFile(lpszFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			return false;
		}
		DWORD dwWrite;
		if (!WriteFile(hFile, m_lpMem, GetSize(), &dwWrite, NULL))
		{
			CloseHandle(hFile);
			return false;
		}
		CloseHandle(hFile);
		return dwWrite == GetSize();
	}

}