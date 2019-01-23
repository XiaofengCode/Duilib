#include "duipub.h"
#include "GifImage.h"

namespace DuiLib{


	CGifImage::CGifImage()
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
		CDuiBuffer buf;
		if (!DuiReadResourceFileData(pstrGifPath, buf))
		{
			return NULL;
		}

		Gdiplus::Image* pImage = LoadGifFromMemory(buf, buf.GetSize());
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
