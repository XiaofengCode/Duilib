#include "stdafx.h"
#include "GifImage.h"

namespace DuiLib{


	CGifImage::CGifImage(CPaintManagerUI* pManager):m_pManager(pManager)
	{
		m_pImage			=	NULL;
		m_pPropertyItem		=	NULL;
		m_nFrameCount		=	0;	
		m_nFramePosition	=	0;	
		m_bIsAutoPlay		=	true;
		m_bIsPlaying		=	false;
		m_nTimerID			=	100;
		m_pParent			=	NULL;
	}

	CGifImage::~CGifImage()
	{
		DeleteImage();
	}

	void CGifImage::SetAutoPlay( bool bIsAuto /*= true */ )
	{
		m_bIsAutoPlay = bIsAuto;
	}

	bool CGifImage::IsPlay() const
	{
		return m_bIsAutoPlay;
	}

	void CGifImage::OnTimer( UINT_PTR idEvent )
	{
		if ( idEvent != m_nTimerID )
			return;
		m_pParent->GetManager()->KillTimer(m_pParent, m_nTimerID );
		m_pParent->Invalidate();

		m_nFramePosition = (++m_nFramePosition) % m_nFrameCount;

		long lPause = ((long*) m_pPropertyItem->value)[m_nFramePosition] * 10;
		if ( lPause == 0 ) lPause = 100;
		m_pParent->GetManager()->SetTimer( m_pParent, m_nTimerID, lPause );
	}

	void CGifImage::InitImage(CControlUI* pParent, LPCTSTR lpszImage)
	{
		DeleteImage();
		m_pParent = pParent;
		m_sImagePath = lpszImage;
		if (m_sImagePath.IsEmpty())
		{
			return;
		}

		m_pImage = LoadGifFromFile(m_sImagePath);
		if ( NULL == m_pImage ) return;
		UINT nCount	= 0;
		nCount	=	m_pImage->GetFrameDimensionsCount();
		GUID* pDimensionIDs	=	new GUID[ nCount ];
		m_pImage->GetFrameDimensionsList( pDimensionIDs, nCount );
		m_nFrameCount	=	m_pImage->GetFrameCount( &pDimensionIDs[0] );
		int nSize		=	m_pImage->GetPropertyItemSize( PropertyTagFrameDelay );
		if (nSize == 0)
		{
			DeleteImage();
			delete  pDimensionIDs;
			return;
		}

		m_pPropertyItem	=	(Gdiplus::PropertyItem*) malloc( nSize );
		m_pImage->GetPropertyItem( PropertyTagFrameDelay, nSize, m_pPropertyItem );
	
		delete  pDimensionIDs;
		pDimensionIDs = NULL;
		if (m_bIsAutoPlay)
		{
			Play();
		}
	}

	void CGifImage::DeleteImage()
	{
		if ( m_pImage != NULL )
		{
			delete m_pImage;
			m_pImage = NULL;
		}

		if ( m_pPropertyItem != NULL )
		{
			free( m_pPropertyItem );
			m_pPropertyItem = NULL;
		}
		m_nFrameCount		=	0;	
		m_nFramePosition	=	0;	
	}

	void CGifImage::DrawFrame( HDC hDC, RECT rcItem )
	{
		if ( NULL == hDC || NULL == m_pImage ) return;
		GUID pageGuid = Gdiplus::FrameDimensionTime;
		Gdiplus::Graphics graphics( hDC );
		graphics.DrawImage( m_pImage, rcItem.left, rcItem.top, rcItem.right-rcItem.left, rcItem.bottom-rcItem.top );
		m_pImage->SelectActiveFrame( &pageGuid, m_nFramePosition );
	}

	Gdiplus::Image* CGifImage::LoadGifFromFile( LPCTSTR pstrGifPath )
	{
		LPBYTE pData = NULL;
		DWORD dwSize = 0;

		do 
		{
			CDuiString sFile = m_pManager->GetResourcePath();
			if( m_pManager->GetResourceZip().IsEmpty() )
			{
				sFile += pstrGifPath;
				HANDLE hFile = ::CreateFile(sFile.GetData(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, \
					FILE_ATTRIBUTE_NORMAL, NULL);
				if( hFile == INVALID_HANDLE_VALUE ) break;
				dwSize = ::GetFileSize(hFile, NULL);
				if( dwSize == 0 ) break;

				DWORD dwRead = 0;
				pData = new BYTE[ dwSize ];
				::ReadFile( hFile, pData, dwSize, &dwRead, NULL );
				::CloseHandle( hFile );

				if( dwRead != dwSize ) {
					delete[] pData;
					pData = NULL;
					break;
				}
			}
			else
			{
				sFile += m_pManager->GetResourceZip();
				HZIP hz = NULL;
				if( m_pManager->IsCachedResourceZip() )
					hz = (HZIP)m_pManager->GetResourceZipHandle();
				else
					hz = OpenZip((void*)sFile.GetData(), 0, 2);
				if( hz == NULL ) break;
				ZIPENTRY ze; 
				int i; 
				if( FindZipItem(hz, pstrGifPath, true, &i, &ze) != 0 ) break;
				dwSize = ze.unc_size;
				if( dwSize == 0 ) break;
				pData = new BYTE[ dwSize ];
				int res = UnzipItem(hz, i, pData, dwSize, 3);
				if( res != 0x00000000 && res != 0x00000600) {
					delete[] pData;
					pData = NULL;
					if( !m_pManager->IsCachedResourceZip() ) CloseZip(hz);
					break;
				}
				if( !m_pManager->IsCachedResourceZip() ) CloseZip(hz);
			}

		} while (0);

		while (!pData)
		{
			//读不到图片, 则直接去读取bitmap.m_lpstr指向的路径
			HANDLE hFile = ::CreateFile(pstrGifPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, \
				FILE_ATTRIBUTE_NORMAL, NULL);
			if( hFile == INVALID_HANDLE_VALUE ) break;
			dwSize = ::GetFileSize(hFile, NULL);
			if( dwSize == 0 ) break;

			DWORD dwRead = 0;
			pData = new BYTE[ dwSize ];
			::ReadFile( hFile, pData, dwSize, &dwRead, NULL );
			::CloseHandle( hFile );

			if( dwRead != dwSize ) {
				delete[] pData;
				pData = NULL;
			}
			break;
		}
		if (!pData)
		{
			return NULL;
		}

		Gdiplus::Image* pImage = LoadGifFromMemory(pData, dwSize);
		delete pData;
		return pImage;
	}

	void CGifImage::Stop()
	{
		if (!m_bIsPlaying)
		{
			return;
		}

		m_pParent->GetManager()->KillTimer(m_pParent, m_nTimerID);
		m_nFramePosition = 0;
		m_pParent->Invalidate();
		m_bIsPlaying = false;
	}

	void CGifImage::Pause()
	{
		if (!m_bIsPlaying || m_pImage == NULL)
		{
			return;
		}

		m_pParent->GetManager()->KillTimer(m_pParent, m_nTimerID);
		m_pParent->Invalidate();
		m_bIsPlaying = false;
	}

	void CGifImage::Play()
	{
		if (m_bIsPlaying || m_pImage == NULL)
		{
			return;
		}

		long lPause = ((long*) m_pPropertyItem->value)[m_nFramePosition] * 10;
		if ( lPause == 0 ) lPause = 100;
		m_pParent->GetManager()->SetTimer( m_pParent, m_nTimerID, lPause );

		m_bIsPlaying = true;
	}

	Gdiplus::Image* CGifImage::LoadGifFromMemory( LPVOID pBuf,size_t dwSize )
	{
		HGLOBAL hMem = ::GlobalAlloc(GMEM_FIXED, dwSize);
		BYTE* pMem = (BYTE*)::GlobalLock(hMem);

		memcpy(pMem, pBuf, dwSize);
		::GlobalUnlock(hMem);

		IStream* pStm = NULL;
		::CreateStreamOnHGlobal(hMem, TRUE, &pStm);
		Gdiplus::Image *pImg = Gdiplus::Image::FromStream(pStm);
		pStm->Release();
		if(!pImg || pImg->GetLastStatus() != Gdiplus::Ok)
		{
			return 0;
		}
		return pImg;
	}

}
