#ifndef __UIBUTTON_H__
#define __UIBUTTON_H__

#pragma once

namespace DuiLib
{
	enum GifStyle
	{
		Non,
		Auto,
		HotRunLeaveBack,
		HotRunLeaveStop,
		LeaveRunHotBack,
		LeaveRunHotStop
	};
	class UILIB_API CButtonUI : public CLabelUI
	{
	public:
		CButtonUI();
		virtual ~CButtonUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		UINT GetControlFlags() const;

		bool Activate();
		void SetEnabled(bool bEnable = true);
		void DoEvent(TEventUI& event);

		void BindTabIndex(int _BindTabIndex);
		void BindTabLayoutName(LPCTSTR _TabLayoutName);
		void BindTriggerTabSel(int _SetSelectIndex = -1);
		void RemoveBindTabIndex();
		int	 GetBindTabLayoutIndex();
		CDuiString GetBindTabLayoutName();

		SIZE EstimateSize(SIZE szAvailable);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void PaintText(HDC hDC);

	protected:
// 		virtual DWORD GetStatus();
// 		UINT m_uButtonState;
		int			m_iBindTabIndex;
		CDuiString	m_sBindTabLayoutName;
		GifStyle	m_nGifStyle;
	};

}	// namespace DuiLib

#endif // __UIBUTTON_H__