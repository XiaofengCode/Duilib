#pragma once
namespace DuiLib{
	class CControlUI;
	class UILIB_API CGifImage
	{
	public:
		CGifImage();
		~CGifImage();
		//Gif����
		void SetAutoPlay(bool bIsAuto = true );
		bool IsAutoPlay() const;
		void OnTimer( UINT_PTR idEvent );
		void Play(bool bBackRun = false);
		void Pause();
		void Stop();
		void InitImage(CControlUI* pParent, LPCTSTR lpszImage);
		void DeleteImage();
		bool IsValid(){return !!m_pImage;}
		void DrawFrame( HDC hDC, RECT rcItem );

	private:
		Gdiplus::Image*	LoadGifFromFile(LPCTSTR pstrGifPath);
		Gdiplus::Image* LoadGifFromMemory( LPVOID pBuf,size_t dwSize );

		Gdiplus::Image	*m_pImage;
		int				m_nFrameCount;				// gifͼƬ��֡��
		int				m_nFramePosition;			// ��ǰ�ŵ��ڼ�֡
		Gdiplus::PropertyItem*	m_pPropertyItem;	// ֡��֮֡����ʱ��
		bool			m_bIsAutoPlay;				// �Ƿ��Զ�����gif
		bool			m_bIsPlaying;
		UINT			m_nTimerID;
		CControlUI*		m_pParent;
		CDuiString		m_sImagePath;
		int				m_nLoopCount;
		int				m_nCurLoop;
		bool			m_bBackRun;
	};
}

