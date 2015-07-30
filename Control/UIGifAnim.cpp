#include "StdAfx.h"
#include "UIGifAnim.h"

///////////////////////////////////////////////////////////////////////////////////////

namespace DuiLib
{

	CGifAnimUI::CGifAnimUI(void)
	{
		m_pGifBkImage			=	NULL;
		m_pGifBkPropertyItem		=	NULL;
		m_nGifBkFrameCount		=	0;	
		m_nGifBkFramePosition	=	0;	
		m_bIsGifBkAutoPlay		=	true;
		m_bIsGifBkAutoSize		=	false;
		m_bIsGifBkPlaying		=	false;

	}


	CGifAnimUI::~CGifAnimUI(void)
	{
		DeleteGifBk();
		m_pManager->KillTimer( this, EVENT_TIEM_ID );

	}

	LPCTSTR CGifAnimUI::GetClass() const
	{
		return _T("GifAnimUI");
	}

	LPVOID CGifAnimUI::GetInterface( LPCTSTR pstrName )
	{
		if( _tcscmp(pstrName, DUI_CTR_GIFANIM) == 0 ) return static_cast<CGifAnimUI*>(this);
		return CLabelUI::GetInterface(pstrName);
	}

	void CGifAnimUI::DoInit()
	{
		InitGifBkImage();
	}

	void CGifAnimUI::DoPaint( HDC hDC, const RECT& rcPaint )
	{
		if( !::IntersectRect( &m_rcPaint, &rcPaint, &m_rcItem ) ) return;
		if ( NULL == m_pGifBkImage )
		{		
			InitGifBkImage();
		}
		DrawGifBkFrame( hDC );
		PaintText(hDC);
	}

	void CGifAnimUI::DoEvent( TEventUI& event )
	{
		if( event.Type == UIEVENT_TIMER )
			OnTimer( (UINT_PTR)event.wParam );
	}

	void CGifAnimUI::SetVisible(bool bVisible /* = true */)
	{
		CLabelUI::SetVisible(bVisible);
		if (bVisible)
			PlayGifBk();
		else
			StopGifBk();
	}

	void CGifAnimUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("bkimage")) == 0 ) SetBkImage(pstrValue);
		else if( _tcscmp(pstrName, _T("autoplay")) == 0 ) {
			SetGifBkAutoPlay(_tcscmp(pstrValue, _T("true")) == 0);
		}
		else if( _tcscmp(pstrName, _T("autosize")) == 0 ) {
			SetAutoSize(_tcscmp(pstrValue, _T("true")) == 0);
		}
		else
			CLabelUI::SetAttribute(pstrName, pstrValue);
	}

	void CGifAnimUI::SetBkImage(LPCTSTR pStrImage)
	{
		if( m_sBkImage == pStrImage || NULL == pStrImage) return;

		m_sBkImage = pStrImage;

		StopGifBk();
		DeleteGifBk();

		Invalidate();

	}

	LPCTSTR CGifAnimUI::GetBkImage()
	{
		return m_sBkImage.GetData();
	}

	void CGifAnimUI::SetGifBkAutoPlay(bool bIsAuto)
	{
		m_bIsGifBkAutoPlay = bIsAuto;
	}

	bool CGifAnimUI::IsGifBkAutoPlay() const
	{
		return m_bIsGifBkAutoPlay;
	}

	void CGifAnimUI::SetAutoSize(bool bIsAuto)
	{
		m_bIsGifBkAutoSize = bIsAuto;
	}

	bool CGifAnimUI::IsAutoSize() const
	{
		return m_bIsGifBkAutoSize;
	}

	void CGifAnimUI::PlayGifBk()
	{
		if (m_bIsGifBkPlaying || m_pGifBkImage == NULL)
		{
			return;
		}

		long lPause = ((long*) m_pGifBkPropertyItem->value)[m_nGifBkFramePosition] * 10;
		if ( lPause == 0 ) lPause = 100;
		m_pManager->SetTimer( this, EVENT_TIEM_ID, lPause );

		m_bIsGifBkPlaying = true;
	}

	void CGifAnimUI::PauseGifBk()
	{
		if (!m_bIsGifBkPlaying || m_pGifBkImage == NULL)
		{
			return;
		}

		m_pManager->KillTimer(this, EVENT_TIEM_ID);
		this->Invalidate();
		m_bIsGifBkPlaying = false;
	}

	void CGifAnimUI::StopGifBk()
	{
		if (!m_bIsGifBkPlaying)
		{
			return;
		}

		m_pManager->KillTimer(this, EVENT_TIEM_ID);
		m_nGifBkFramePosition = 0;
		this->Invalidate();
		m_bIsGifBkPlaying = false;
	}

	void CGifAnimUI::InitGifBkImage()
	{
		m_pGifBkImage = LoadGifFromFile(GetBkImage());
		if ( NULL == m_pGifBkImage ) return;
		UINT nCount	= 0;
		nCount	=	m_pGifBkImage->GetFrameDimensionsCount();
		GUID* pDimensionIDs	=	new GUID[ nCount ];
		m_pGifBkImage->GetFrameDimensionsList( pDimensionIDs, nCount );
		m_nGifBkFrameCount	=	m_pGifBkImage->GetFrameCount( &pDimensionIDs[0] );
		int nSize		=	m_pGifBkImage->GetPropertyItemSize( PropertyTagFrameDelay );
		m_pGifBkPropertyItem	=	(Gdiplus::PropertyItem*) malloc( nSize );
		m_pGifBkImage->GetPropertyItem( PropertyTagFrameDelay, nSize, m_pGifBkPropertyItem );
		delete  pDimensionIDs;
		pDimensionIDs = NULL;

		if (m_bIsGifBkAutoSize)
		{
			SetFixedWidth(m_pGifBkImage->GetWidth());
			SetFixedHeight(m_pGifBkImage->GetHeight());
		}
		if (m_bIsGifBkAutoPlay)
		{
			PlayGifBk();
		}
	}

	void CGifAnimUI::DeleteGifBk()
	{
		if ( m_pGifBkImage != NULL )
		{
			delete m_pGifBkImage;
			m_pGifBkImage = NULL;
		}

		if ( m_pGifBkPropertyItem != NULL )
		{
			free( m_pGifBkPropertyItem );
			m_pGifBkPropertyItem = NULL;
		}
		m_nGifBkFrameCount		=	0;	
		m_nGifBkFramePosition	=	0;	
	}

	void CGifAnimUI::OnTimer( UINT_PTR idEvent )
	{
		if ( idEvent != EVENT_TIEM_ID )
			return;
		m_pManager->KillTimer( this, EVENT_TIEM_ID );
		this->Invalidate();

		m_nGifBkFramePosition = (++m_nGifBkFramePosition) % m_nGifBkFrameCount;

		long lPause = ((long*) m_pGifBkPropertyItem->value)[m_nGifBkFramePosition] * 10;
		if ( lPause == 0 ) lPause = 100;
		m_pManager->SetTimer( this, EVENT_TIEM_ID, lPause );
	}

	void CGifAnimUI::DrawGifBkFrame( HDC hDC )
	{
		if ( NULL == hDC || NULL == m_pGifBkImage ) return;
		GUID pageGuid = Gdiplus::FrameDimensionTime;
		Gdiplus::Graphics graphics( hDC );
		graphics.DrawImage( m_pGifBkImage, m_rcItem.left, m_rcItem.top, m_rcItem.right-m_rcItem.left, m_rcItem.bottom-m_rcItem.top );
		m_pGifBkImage->SelectActiveFrame( &pageGuid, m_nGifBkFramePosition );
	}

	Gdiplus::Image* CGifAnimUI::LoadGifFromFile(LPCTSTR pstrGifPath)
	{
		LPBYTE pData = NULL;
		DWORD dwSize = 0;

		do 
		{
			CDuiString sFile = CPaintManagerUI::GetResourcePath();
			if( CPaintManagerUI::GetResourceZip().IsEmpty() ) {
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
			else {
				sFile += CPaintManagerUI::GetResourceZip();
				HZIP hz = NULL;
				if( CPaintManagerUI::IsCachedResourceZip() ) hz = (HZIP)CPaintManagerUI::GetResourceZipHandle();
				else hz = OpenZip((void*)sFile.GetData(), 0, 2);
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
					if( !CPaintManagerUI::IsCachedResourceZip() ) CloseZip(hz);
					break;
				}
				if( !CPaintManagerUI::IsCachedResourceZip() ) CloseZip(hz);
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

	Gdiplus::Image* CGifAnimUI::LoadGifFromMemory( LPVOID pBuf,size_t dwSize )
	{
		HGLOBAL hMem = ::GlobalAlloc(GMEM_FIXED, dwSize);
		BYTE* pMem = (BYTE*)::GlobalLock(hMem);

		memcpy(pMem, pBuf, dwSize);

		IStream* pStm = NULL;
		::CreateStreamOnHGlobal(hMem, TRUE, &pStm);
		Gdiplus::Image *pImg = Gdiplus::Image::FromStream(pStm);
		if(!pImg || pImg->GetLastStatus() != Gdiplus::Ok)
		{
			pStm->Release();
			::GlobalUnlock(hMem);
			return 0;
		}
		return pImg;
	}


}