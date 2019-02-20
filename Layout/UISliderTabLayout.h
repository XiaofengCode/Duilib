#ifndef __SLIDERTABLAYOUT_H__
#define __SLIDERTABLAYOUT_H__

#pragma once


namespace DuiLib {
	
	class UILIB_API CSliderTabLayoutUI : public CTabLayoutUI
	{
	public:
		LBIND_CLASS_DEFINE(CSliderTabLayoutUI, CTabLayoutUI);
		CSliderTabLayoutUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		void DoEvent(TEventUI& event);
		void OnTimer( int nTimerID );

		void SetPos(RECT rc);
		bool SelectItem( int iIndex );

		void OnSliderStep();

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		virtual void LineUp();

		virtual void LineDown();

	protected:
		int         m_iCurFrame;
		bool        m_bIsVertical;
		int         m_nPositiveDirection;

		bool        m_bAnimat;
		bool        m_bAnimating;

		RECT        m_rcCurPos;
		RECT        m_rcNextPos;

		CControlUI* m_pCurPage;			// 保存当前显示的页面
		CControlUI* m_pNextPage;		// 保存下一页面

		int			m_nEllapse;
		int			m_nFrameCount;

		bool		m_bScrollWheel;
		enum
		{
			TIMER_ANIMATION_ID = 1,
			ANIMATION_ELLAPSE = 10,
			ANIMATION_FRAME_COUNT = 15
		};
	};

} // namespace DuiLib

#endif // __UISlider_H__