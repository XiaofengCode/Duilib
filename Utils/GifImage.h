#pragma once
namespace DuiLib{
	class CControlUI;
	class UILIB_API CGifImage
	{
	public:
		CGifImage(CPaintManagerUI* pManager);
		~CGifImage();
		//Gif����
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
		UINT			m_nFrameCount;				// gifͼƬ��֡��
		UINT			m_nFramePosition;			// ��ǰ�ŵ��ڼ�֡
		Gdiplus::PropertyItem*	m_pPropertyItem;	// ֡��֮֡����ʱ��
		bool			m_bIsAutoPlay;				// �Ƿ��Զ�����gif
		bool			m_bIsPlaying;
		UINT			m_nTimerID;
		CControlUI*		m_pParent;
		CDuiString		m_sImagePath;
		CPaintManagerUI* m_pManager;
	};
}

