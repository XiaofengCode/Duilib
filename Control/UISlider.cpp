#include "..\duipub.h"
#include "UISlider.h"

namespace DuiLib
{
	CSliderUI::CSliderUI() : m_uButtonState(0), m_nStep(1),m_bSendMove(false)
	{
		m_attrs.AddKeyword(DUI_ATTR_POS_THUMB);
		m_bButton = false;
		m_uTextStyle = DT_SINGLELINE | DT_CENTER;
		m_szThumb.cx = m_szThumb.cy = 10;
	}

	LPCTSTR CSliderUI::GetClass() const
	{
		return _T("SliderUI");
	}

	UINT CSliderUI::GetControlFlags() const
	{
		if( IsEnabled() ) return UIFLAG_SETCURSOR;
		else return 0;
	}

	LPVOID CSliderUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_SLIDER) == 0 ) return static_cast<CSliderUI*>(this);
		return CProgressUI::GetInterface(pstrName);
	}

	void CSliderUI::SetEnabled(bool bEnable)
	{
		CControlUI::SetEnabled(bEnable);
		if( !IsEnabled() ) {
			m_uButtonState = 0;
		}
	}

	int CSliderUI::GetChangeStep()
	{
		return m_nStep;
	}

	void CSliderUI::SetChangeStep(int step)
	{
		m_nStep = step;
	}

	void CSliderUI::SetThumbSize(SIZE szXY)
	{
		m_szThumb = szXY;
	}

	RECT CSliderUI::GetThumbRect() const
	{
		if( IsHorizontal() ) {
			int left = m_rcItem.left + (LONGLONG)(m_rcItem.right - m_rcItem.left - m_szThumb.cx) * (m_nValue - m_nMin) / (m_nMax - m_nMin);
			int top = (m_rcItem.bottom + m_rcItem.top - m_szThumb.cy) / 2;
			return CDuiRect(left, top, left + m_szThumb.cx, top + m_szThumb.cy); 
		}
		else {
			int left = (m_rcItem.right + m_rcItem.left - m_szThumb.cx) / 2;
			int top = m_rcItem.bottom - m_szThumb.cy - (LONGLONG)(m_rcItem.bottom - m_rcItem.top - m_szThumb.cy) * (m_nValue - m_nMin) / (m_nMax - m_nMin);
			return CDuiRect(left, top, left + m_szThumb.cx, top + m_szThumb.cy); 
		}
	}

	const CDuiImage CSliderUI::GetThumbImage() const
	{
		return m_attrs.GetImage(DUI_ATTR_POS_THUMB DUI_ATTR_IMAGE);
	}

	void CSliderUI::SetThumbImage(LPCTSTR pStrImage)
	{
		CDuiString sImageOld = m_attrs.GetString(DUI_ATTR_POS_THUMB DUI_ATTR_IMAGE);
		if( sImageOld == pStrImage )
			return;

		if (!m_attrs.SetAttribute(DUI_ATTR_POS_THUMB DUI_ATTR_IMAGE, pStrImage))
			return;

		const CDuiImage img = GetBkImage();
		if (img)
		{
			Invalidate();
		}
	}

	const CDuiImage CSliderUI::GetThumbHotImage() const
	{
		return m_attrs.GetImage(DUI_ATTR_POS_THUMB DUI_ATTR_STATUS_HOT DUI_ATTR_IMAGE);
	}

	void CSliderUI::SetThumbHotImage(LPCTSTR pStrImage)
	{
		CDuiString sImageOld = m_attrs.GetString(DUI_ATTR_POS_THUMB DUI_ATTR_STATUS_HOT DUI_ATTR_IMAGE);
		if( sImageOld == pStrImage )
			return;

		if (!m_attrs.SetAttribute(DUI_ATTR_POS_THUMB DUI_ATTR_STATUS_HOT DUI_ATTR_IMAGE, pStrImage))
			return;

		const CDuiImage img = GetBkImage();
		if (img)
		{
			Invalidate();
		}
	}

	const CDuiImage CSliderUI::GetThumbPushedImage() const
	{
		return m_attrs.GetImage(DUI_ATTR_POS_THUMB DUI_ATTR_STATUS_PUSHED DUI_ATTR_IMAGE);
	}

	void CSliderUI::SetThumbPushedImage(LPCTSTR pStrImage)
	{
		CDuiString sImageOld = m_attrs.GetString(DUI_ATTR_POS_THUMB DUI_ATTR_STATUS_PUSHED DUI_ATTR_IMAGE);
		if( sImageOld == pStrImage )
			return;

		if (!m_attrs.SetAttribute(DUI_ATTR_POS_THUMB DUI_ATTR_STATUS_PUSHED DUI_ATTR_IMAGE, pStrImage))
			return;

		const CDuiImage img = GetBkImage();
		if (img)
		{
			Invalidate();
		}
	}

	void CSliderUI::SetValue(int nValue) //2014.7.28 redrain  ��������ڻ�������ʱ�����յ�SetValue��Ӱ�죬���绬���ı����ֵĽ��ȣ�������Ϊ�ⲿһֱ����SetValue���������޷���������
	{
		if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) 
			return;
		CProgressUI::SetValue(nValue);
	}
	void CSliderUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pParent != NULL ) m_pParent->DoEvent(event);
			else CProgressUI::DoEvent(event);
			return;
		}

		if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK )
		{
			if( IsEnabled() ) {//2014.7.28 redrain ע�͵�ԭ���Ĵ��룬������Щ����������Slider��������굯��ʱ�Ÿı们���λ��
				m_uButtonState |= UISTATE_CAPTURED;

				int nValue;

				if( IsHorizontal() ) {
					if( event.ptMouse.x >= m_rcItem.right - m_szThumb.cx / 2 ) nValue = m_nMax;
					else if( event.ptMouse.x <= m_rcItem.left + m_szThumb.cx / 2 ) nValue = m_nMin;
					else nValue = m_nMin + (m_nMax - m_nMin) * (event.ptMouse.x - m_rcItem.left - m_szThumb.cx / 2 ) / (m_rcItem.right - m_rcItem.left - m_szThumb.cx);
				}
				else {
					if( event.ptMouse.y >= m_rcItem.bottom - m_szThumb.cy / 2 ) nValue = m_nMin;
					else if( event.ptMouse.y <= m_rcItem.top + m_szThumb.cy / 2  ) nValue = m_nMax;
					else nValue = m_nMin + (m_nMax - m_nMin) * (m_rcItem.bottom - event.ptMouse.y - m_szThumb.cy / 2 ) / (m_rcItem.bottom - m_rcItem.top - m_szThumb.cy);
				}
				if(m_nValue !=nValue && nValue>=m_nMin && nValue<=m_nMax)
				{
					m_nValue =nValue;
					Invalidate();
				}
			}
				return;
		}

// 		if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK )
// 		{
// 			if( IsEnabled() ) {
// 				RECT rcThumb = GetThumbRect();
// 				if( ::PtInRect(&rcThumb, event.ptMouse) ) {
// 					m_uButtonState |= UISTATE_CAPTURED;
// 				}
// 			}
// 			return;
// 		}
		if( event.Type == UIEVENT_BUTTONUP )
		{
			//Eric Modified 2015-7-13
			//ֻ��Enableʱ��Ч
			if (IsEnabled())
			{
				int nValue;
				if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
					m_uButtonState &= ~UISTATE_CAPTURED;
				}
				if( IsHorizontal() ) {
					if( event.ptMouse.x >= m_rcItem.right - m_szThumb.cx / 2 ) nValue = m_nMax;
					else if( event.ptMouse.x <= m_rcItem.left + m_szThumb.cx / 2 ) nValue = m_nMin;
					else nValue = m_nMin + (LONGLONG)(m_nMax - m_nMin) * (event.ptMouse.x - m_rcItem.left - m_szThumb.cx / 2) / (m_rcItem.right - m_rcItem.left - m_szThumb.cx);
				}
				else {
					if( event.ptMouse.y >= m_rcItem.bottom - m_szThumb.cy / 2 ) nValue = m_nMin;
					else if( event.ptMouse.y <= m_rcItem.top + m_szThumb.cy / 2  ) nValue = m_nMax;
					else nValue = m_nMin + (LONGLONG)(m_nMax - m_nMin) * (m_rcItem.bottom - event.ptMouse.y - m_szThumb.cy / 2) / (m_rcItem.bottom - m_rcItem.top - m_szThumb.cy);
				}
				if(/*m_nValue !=nValue && 2014.7.28 redrain ���ע�ͺܹؼ�����������������϶������޷�����DUI_MSGTYPE_VALUECHANGED��Ϣ*/nValue>=m_nMin && nValue<=m_nMax)
				{
					m_nValue =nValue;
					m_pManager->SendNotify(this, DUI_MSGTYPE_VALUECHANGED, m_nValue);//����ǰλ�ô��ݸ��ϲ�, Eric modified 2015-7-13
					Invalidate();
				}
			}
			
			return;
		}
		if( event.Type == UIEVENT_CONTEXTMENU )
		{
			return;
		}
		if( event.Type == UIEVENT_SCROLLWHEEL ) 
		{
			short nDalta = (short)HIWORD(event.wParam);
			if (nDalta < 0)
			{
				SetValue(GetValue() - GetChangeStep());
				m_pManager->SendNotify(this, DUI_MSGTYPE_VALUECHANGED, m_nValue);//����ǰλ�ô��ݸ��ϲ�, Eric modified 2015-7-13
				return;
			}
			else
			{
				SetValue(GetValue() + GetChangeStep());
				m_pManager->SendNotify(this, DUI_MSGTYPE_VALUECHANGED, m_nValue);//����ǰλ�ô��ݸ��ϲ�, Eric modified 2015-7-13
				return;
			}
// 			switch( LOWORD(event.wParam) ) {
// 		case SB_LINEUP:
// 			SetValue(GetValue() + GetChangeStep());
// 			m_pManager->SendNotify(this, DUI_MSGTYPE_VALUECHANGED, m_nValue);//����ǰλ�ô��ݸ��ϲ�, Eric modified 2015-7-13
// 			return;
// 		case SB_LINEDOWN:
// 			SetValue(GetValue() - GetChangeStep());
// 			m_pManager->SendNotify(this, DUI_MSGTYPE_VALUECHANGED, m_nValue);//����ǰλ�ô��ݸ��ϲ�, Eric modified 2015-7-13
// 			return;
// 			}
		}
		if( event.Type == UIEVENT_MOUSEMOVE )
		{
			if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {//2014.7.28 redrain ��д�����Ϣ�ж���Slider����DUI_MSGTYPE_VALUECHANGED_MOVE��Ϣ�������ڻ�������Ҳ������Ϣ���������ڸı�����ʱ��һ�߻����Ϳ���һ�߸ı�����
				if( IsHorizontal() ) {
					if( event.ptMouse.x >= m_rcItem.right - m_szThumb.cx / 2 ) m_nValue = m_nMax;
					else if( event.ptMouse.x <= m_rcItem.left + m_szThumb.cx / 2 ) m_nValue = m_nMin;
					else m_nValue = m_nMin + (m_nMax - m_nMin) * (event.ptMouse.x - m_rcItem.left - m_szThumb.cx / 2 ) / (m_rcItem.right - m_rcItem.left - m_szThumb.cx);
				}
				else {
					if( event.ptMouse.y >= m_rcItem.bottom - m_szThumb.cy / 2 ) m_nValue = m_nMin;
					else if( event.ptMouse.y <= m_rcItem.top + m_szThumb.cy / 2  ) m_nValue = m_nMax;
					else m_nValue = m_nMin + (m_nMax - m_nMin) * (m_rcItem.bottom - event.ptMouse.y - m_szThumb.cy / 2 ) / (m_rcItem.bottom - m_rcItem.top - m_szThumb.cy);
				}
				if (m_bSendMove)
					m_pManager->SendNotify(this, DUI_MSGTYPE_VALUECHANGED_MOVE);
				Invalidate();
			}

			// Generate the appropriate mouse messages
			POINT pt = event.ptMouse;
			RECT rcThumb = GetThumbRect();
			if( IsEnabled() && ::PtInRect(&rcThumb, event.ptMouse) ) {

				m_uButtonState |= UISTATE_HOT;
				Invalidate();
			}
			else
			{
				m_uButtonState &= ~UISTATE_HOT;
				Invalidate();
			}
			return;
		}
// 		if( event.Type == UIEVENT_MOUSEMOVE )
// 		{
// 			if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
// 				if( m_bHorizontal ) {
// 					if( event.ptMouse.x >= m_rcItem.right - m_szThumb.cx / 2 ) m_nValue = m_nMax;
// 					else if( event.ptMouse.x <= m_rcItem.left + m_szThumb.cx / 2 ) m_nValue = m_nMin;
// 					else m_nValue = m_nMin + (m_nMax - m_nMin) * (event.ptMouse.x - m_rcItem.left - m_szThumb.cx / 2 ) / (m_rcItem.right - m_rcItem.left - m_szThumb.cx);
// 				}
// 				else {
// 					if( event.ptMouse.y >= m_rcItem.bottom - m_szThumb.cy / 2 ) m_nValue = m_nMin;
// 					else if( event.ptMouse.y <= m_rcItem.top + m_szThumb.cy / 2  ) m_nValue = m_nMax;
// 					else m_nValue = m_nMin + (m_nMax - m_nMin) * (m_rcItem.bottom - event.ptMouse.y - m_szThumb.cy / 2 ) / (m_rcItem.bottom - m_rcItem.top - m_szThumb.cy);
// 				}
// 				Invalidate();
// 			}
// 			return;
// 		}
		if( event.Type == UIEVENT_SETCURSOR && IsEnabled())
		{
			RECT rcThumb = GetThumbRect();
			if( IsEnabled()) {
				::SetCursor(::LoadCursor(NULL, IDC_HAND));
				return;
			}
		}
// 		if( event.Type == UIEVENT_SETCURSOR )
// 		{
// 			RECT rcThumb = GetThumbRect();
// 			if( IsEnabled() && ::PtInRect(&rcThumb, event.ptMouse) ) {
// 				::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
// 				return;
// 			}
// 		}
		if( event.Type == UIEVENT_MOUSEENTER )
		{//2014.7.28 redrain ֻ������ڻ���ķ�Χ�ڲű�ΪUISTATE_HOT
// 			if( IsEnabled() ) {
// 	  			m_uButtonState |= UISTATE_HOT;
// 				Invalidate();
// 			}
// 			return;
		}
		if( event.Type == UIEVENT_MOUSELEAVE )
		{
			if( IsEnabled() ) {
				m_uButtonState &= ~UISTATE_HOT;
				Invalidate();
			}
			return;
		}
		if (event.Type == UIEVENT_KILLFOCUS)
		{
			if ((m_uButtonState & UISTATE_CAPTURED) != 0)
			{
				m_uButtonState &= ~UISTATE_CAPTURED;
				m_pManager->SendNotify(this, DUI_MSGTYPE_VALUECHANGED, m_nValue);
			}
			//return;
		}
		__super::DoEvent(event);
	}

	void CSliderUI::SetCanSendMove(bool bCanSend) //2014.7.28 redrain
	{
		m_bSendMove = bCanSend;
	}
	bool CSliderUI::GetCanSendMove() const //2014.7.28 redrain
	{
		return m_bSendMove;
	}

	void CSliderUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcsicmp(pstrName, _T("thumbimage")) == 0 ) SetThumbImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("thumbhotimage")) == 0 ) SetThumbHotImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("thumbpushedimage")) == 0 ) SetThumbPushedImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("thumbsize")) == 0 ) {
			SIZE szXY = {0};
			LPTSTR pstr = NULL;
			szXY.cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
			szXY.cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr); 
			SetThumbSize(szXY);
		}
		else if( _tcsicmp(pstrName, _T("step")) == 0 ) {
			SetChangeStep(_ttoi(pstrValue));
		}
		else if( _tcsicmp(pstrName, _T("sendmove")) == 0 ) {
			SetCanSendMove(_tcsicmp(pstrValue, _T("true")) == 0);
		}
		else CProgressUI::SetAttribute(pstrName, pstrValue);
	}

	void CSliderUI::PaintForeImage(HDC hDC)
	{
		CProgressUI::PaintForeImage(hDC);

		double S = GetScaleDpi();
		RECT rcThumb = GetThumbRect();
		rcThumb.left -= m_rcItem.left;
		rcThumb.top -= m_rcItem.top;
		rcThumb.right -= m_rcItem.left;
		rcThumb.bottom -= m_rcItem.top;
		if( (m_uButtonState & UISTATE_CAPTURED) != 0 )
		{
			CDuiImage img = GetThumbPushedImage();
			if( img )
			{
				img[0].m_rcDst = rcThumb;
				if( !DrawImage(hDC, img[0]) )
					SetThumbPushedImage(NULL);
				else
					return;
			}
		}
		else if( (m_uButtonState & UISTATE_HOT) != 0 )
		{
			CDuiImage img = GetThumbHotImage();
			if( img )
			{
				img[0].m_rcDst = rcThumb;
				if( !DrawImage(hDC, img[0]) )
					SetThumbHotImage(NULL);
				else
					return;
			}
		}

		CDuiImage img = GetThumbImage();
		if( img )
		{
			img[0].m_rcDst = rcThumb;
			if( !DrawImage(hDC, img[0]) )
				SetThumbImage(NULL);
			else
				return;
		}
	}
}
