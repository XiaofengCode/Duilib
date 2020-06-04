#include "..\duipub.h"
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
		m_nLoopCount = 0;
		m_nCurLoop = 0;
		m_bBackRun = false;
	}

	CGifImage::~CGifImage()
	{
		DeleteImage();
	}

	void CGifImage::SetAutoPlay( bool bIsAuto /*= true */ )
	{
		m_bIsAutoPlay = bIsAuto;
	}

	bool CGifImage::IsAutoPlay() const
	{
		return m_bIsAutoPlay;
	}

	void CGifImage::OnTimer( UINT_PTR idEvent )
	{
		if ( idEvent != m_nTimerID )
			return;
		long lPause = ((long*)m_pPropertyItem->value)[m_nFramePosition] * 10;
		m_pParent->GetManager()->KillTimer(m_pParent, m_nTimerID );
		if (m_nLoopCount == 0)
		{
			if (m_bBackRun)
			{
				--m_nFramePosition;
				if (m_nFramePosition < 0)
					m_nFramePosition = m_nFrameCount - 1;
			}
			else
			{
				m_nFramePosition = (++m_nFramePosition) % m_nFrameCount;
			}
		}
		else if (!m_bBackRun && m_nCurLoop < m_nLoopCount)
		{
			++m_nFramePosition;
			if (m_nFramePosition >= m_nFrameCount)
			{
				m_nCurLoop++;
				if (m_nCurLoop < m_nLoopCount)
				{
					m_nFramePosition = 0;
				}
				else
				{
					m_nFramePosition = m_nFrameCount - 1;
					m_nCurLoop = 0;
				}
			}
		}
		else if (m_bBackRun)
		{
			if (m_nFramePosition == 0)
			{
				if (m_nCurLoop > 0)
				{
					m_nCurLoop--;
					m_nFramePosition = m_nFrameCount - 1;
				}
				else
				{
					m_bIsPlaying = false;
					return;
				}
			}
			else
				--m_nFramePosition;
		}
		else
		{
			m_bIsPlaying = false;
			return;
		}
		m_pParent->Invalidate();

		
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

		// get loop count
		UINT uSize = m_pImage->GetPropertyItemSize(PropertyTagLoopCount);
		Gdiplus::PropertyItem* pPropLoopCount = (Gdiplus::PropertyItem*)new char[uSize];
		m_pImage->GetPropertyItem(PropertyTagLoopCount, uSize, pPropLoopCount);
		m_nLoopCount = *((SHORT*)pPropLoopCount->value);

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
		Gdiplus::Graphics graphics(hDC);
		m_pImage->SelectActiveFrame(&pageGuid, m_nFramePosition);
		graphics.DrawImage( m_pImage, rcItem.left, rcItem.top, rcItem.right-rcItem.left + 1, rcItem.bottom-rcItem.top + 1 );
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
		m_nCurLoop = 0;
		m_bBackRun = false;
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

	void CGifImage::Play(bool bBackRun)
	{
		if (m_pImage == NULL)
		{
			return;
		}
		m_bBackRun = bBackRun;
		if (m_bIsPlaying)
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
