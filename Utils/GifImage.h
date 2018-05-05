#pragma once
namespace DuiLib{
	class CControlUI;
	class UILIB_API CGifImage
	{
	public:
		CGifImage(CPaintManagerUI* pManager);
		~CGifImage();
		//Gif背景
		void SetAutoPlay(bool bIsAuto = true );
		bool IsPlay() const;
		void OnTimer( UINT_PTR idEvent );
		void Play();
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
		UINT			m_nFrameCount;				// gif图片总帧数
		UINT			m_nFramePosition;			// 当前放到第几帧
		Gdiplus::PropertyItem*	m_pPropertyItem;	// 帧与帧之间间隔时间
		bool			m_bIsAutoPlay;				// 是否自动播放gif
		bool			m_bIsPlaying;
		UINT			m_nTimerID;
		CControlUI*		m_pParent;
		CDuiString		m_sImagePath;
		CPaintManagerUI* m_pManager;
	};
}

