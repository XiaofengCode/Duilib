#include "duipub.h"
#include "FileHelper.h"

namespace DuiLib {
	bool _Failed(LPCTSTR pstrError, LPCTSTR pstrLocation /*= NULL*/)
	{
		OutputDebugString(pstrError);
		return false;
	}

	BOOL DuiReadFileData(LPCTSTR pstrFilename, CDuiBuffer& buffer)
	{
		HANDLE hFile = CreateFile(CPaintManagerUI::GetResourcePath() + pstrFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if( hFile == INVALID_HANDLE_VALUE )
		{
			hFile = CreateFile(pstrFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if( hFile == INVALID_HANDLE_VALUE )
			{
				hFile = CreateFile(CPaintManagerUI::GetInstancePath() + pstrFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				if( hFile == INVALID_HANDLE_VALUE )
				{
					return _Failed(_T("Error opening file"));
				}
			}
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
			DuiReadFileData(pstrFilename, buffer);
		}
		else
		{
			HZIP hz = NULL;
			if( CPaintManagerUI::IsCachedResourceZip() )
				hz = (HZIP)CPaintManagerUI::GetResourceZipHandle();
			if (hz == NULL)
			{
				hz = OpenZip((void*)(LPCTSTR)(CPaintManagerUI::GetResourcePath() + CPaintManagerUI::GetResourceZip()), 0, 2);
				if (hz == NULL)
				{
					hz = OpenZip((void*)(LPCTSTR)CPaintManagerUI::GetResourceZip(), 0, 2);
					if (hz == NULL)
					{
						hz = OpenZip((void*)(LPCTSTR)(CPaintManagerUI::GetInstancePath() + CPaintManagerUI::GetResourceZip()), 0, 2);
						if (hz == NULL)
							return _Failed(_T("Error opening zip file"));
					}
				}
			}

			ZIPENTRY ze; 
			int i; 
			if( FindZipItem(hz, pstrFilename, true, &i, &ze) != 0 ) return _Failed(_T("Could not find ziped file"));
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
}