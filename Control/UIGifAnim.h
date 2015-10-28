#ifndef GifAnimUI_h__
#define GifAnimUI_h__

#pragma once

namespace DuiLib
{
	class CControl;

#define EVENT_TIEM_ID	100

	class UILIB_API CGifAnimUI : public CLabelUI
	{
	public:
		CGifAnimUI(void);
		~CGifAnimUI(void);

		LPCTSTR	GetClass() const;
		LPVOID	GetInterface(LPCTSTR pstrName);
		virtual void	DoInit();
		virtual void	DoPaint(HDC hDC, const RECT& rcPaint);
		virtual void	DoEvent(TEventUI& event);
		virtual void	SetVisible(bool bVisible = true );
		virtual void	SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void	SetBkImage(LPCTSTR pStrImage);
		LPCTSTR GetBkImage();

		void	SetGifBkAutoPlay(bool bIsAuto = true );
		bool	IsGifBkAutoPlay() const;
		void	SetAutoSize(bool bIsAuto = true );
		bool	IsAutoSize() const;
		void	PlayGif();
		void	PauseGif();
		void	StopGif();

	private:
		void	InitGifBkImage();
		void	DeleteGifBk();
		void    OnTimer( UINT_PTR idEvent );
		void	DrawGifBkFrame( HDC hDC );		// ����GIFÿ֡
		Gdiplus::Image*	LoadGifFromFile(LPCTSTR pstrGifPath);
		Gdiplus::Image* LoadGifFromMemory( LPVOID pBuf,size_t dwSize );
	private:
		Gdiplus::Image	*m_pGifBkImage;
		UINT			m_nGifBkFrameCount;				// gifͼƬ��֡��
		UINT			m_nGifBkFramePosition;			// ��ǰ�ŵ��ڼ�֡
		Gdiplus::PropertyItem*	m_pGifBkPropertyItem;	// ֡��֮֡����ʱ��

		CDuiString		m_sBkImage;
		bool			m_bIsGifBkAutoPlay;				// �Ƿ��Զ�����gif
		bool			m_bIsGifBkAutoSize;				// �Ƿ��Զ�����ͼƬ���ô�С
		bool			m_bIsGifBkPlaying;

	};
}

#endif // GifAnimUI_h__
