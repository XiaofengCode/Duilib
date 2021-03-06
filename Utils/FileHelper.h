#pragma once

namespace DuiLib {
	bool _Failed( LPCTSTR pstrError, LPCTSTR pstrLocation = NULL);


	class CDuiBuffer
	{
	public:
		CDuiBuffer(DWORD dwSize = 0);

		~CDuiBuffer();

		DWORD GetSize(){return m_dwSize;}

		BOOL Resize(DWORD dwNewSize);

		void Release();

		bool SaveAs(LPCTSTR lpszFileName);

		operator LPBYTE()
		{
			return (LPBYTE)m_lpMem;
		}

		operator LPVOID()
		{
			return m_lpMem;
		}

		operator bool ()
		{
			return !!m_lpMem;
		}

		LPVOID m_lpMem;
		DWORD m_dwSize;
	};

	CDuiString DuiGetAbsFilePath(LPCTSTR pstrFilename);
	//std::string DuiGetAbsFilePath(LPCSTR pstrFilename);
	BOOL DuiReadFileData(LPCTSTR pstrFilename, CDuiBuffer& buffer);
	BOOL DuiReadResourceFileData(LPCTSTR pstrFilename, CDuiBuffer& buffer);
}
