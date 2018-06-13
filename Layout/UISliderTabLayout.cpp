#include "duipub.h"
#include "UISliderTabLayout.h"

namespace DuiLib {
//////////////////////////////////////////////////////////////////////////
// CSliderTabLayoutUI
	CSliderTabLayoutUI::CSliderTabLayoutUI() : 
	m_bIsVertical( false ),
	m_bAnimat(false),
	m_iCurFrame( 0 ),
	m_nEllapse(ANIMATION_ELLAPSE),
	m_nFrameCount(ANIMATION_FRAME_COUNT),
	m_bAnimating(false),
	m_bScrollWheel(false)
	{
	}

	LPCTSTR CSliderTabLayoutUI::GetClass() const
	{
		return _T("SliderTabLayoutUI");
	}

	LPVOID CSliderTabLayoutUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, _T("SliderTabLayoutUI")) == 0 ) 
			return static_cast<CSliderTabLayoutUI*>(this);
		return CTabLayoutUI::GetInterface(pstrName);
	}

	bool CSliderTabLayoutUI::SelectItem( int iIndex )
	{
		if( iIndex < 0 || iIndex >= m_items.GetSize() ) return false;
		if( iIndex == m_iCurSel ) return true;
		if( iIndex > m_iCurSel ) m_nPositiveDirection = -1;
		if( iIndex < m_iCurSel ) m_nPositiveDirection = 1;

		m_bAnimat = true;
		m_iCurFrame = 0;

		RECT rcInset = GetInset();
		m_rcCurPos = GetPos();

		m_rcCurPos.left += rcInset.left;
		m_rcCurPos.top += rcInset.top;
		m_rcCurPos.right -= rcInset.right;
		m_rcCurPos.bottom -= rcInset.bottom;

		m_rcNextPos = m_rcCurPos;

		if( !m_bIsVertical )  //横向
		{
			m_rcNextPos.left = m_rcCurPos.left - (m_rcCurPos.right - m_rcCurPos.left) * m_nPositiveDirection;
			m_rcNextPos.right = m_rcCurPos.right - (m_rcCurPos.right - m_rcCurPos.left) * m_nPositiveDirection;
		}
		else
		{
			m_rcNextPos.top = m_rcCurPos.top - (m_rcCurPos.bottom - m_rcCurPos.top) * m_nPositiveDirection;
			m_rcNextPos.bottom = m_rcCurPos.bottom - (m_rcCurPos.bottom - m_rcCurPos.top) * m_nPositiveDirection;
		}

		int iOldSel = m_iCurSel;
		m_iCurSel = iIndex;
		for( int it = 0; it < m_items.GetSize(); it++ )
		{
			if( it == iIndex ) {
				m_pNextPage = GetItemAt(it);
				m_pNextPage->SetPos(m_rcNextPos);
				m_pNextPage->SetVisible(true);
			}
			else if( it == iOldSel )
			{
				m_pCurPage = GetItemAt(it);
				m_pCurPage->SetVisible(true);
			}
			else
				GetItemAt(it)->SetVisible(false);
		}

		m_pManager->SetTimer( this, TIMER_ANIMATION_ID, m_nEllapse );
		if( m_pManager != NULL ) {
			//m_pManager->SetNextTabControl();
			m_pManager->SendNotify(this, DUI_MSGTYPE_TABSELECT, m_iCurSel, iOldSel);
		}
		return true;
	}

	void CSliderTabLayoutUI::DoEvent(TEventUI& event)
	{
		if( event.Type == UIEVENT_TIMER ) 
		{
			OnTimer( event.wParam );
		}
		else if(m_bScrollWheel && event.Type == UIEVENT_SCROLLWHEEL )
		{
			switch( LOWORD(event.wParam) ) {
			case SB_LINEUP:
				LineUp();
				return;
			case SB_LINEDOWN:
				LineDown();
				return;
			}
		}
		CTabLayoutUI::DoEvent(event);
	}

	void CSliderTabLayoutUI::OnTimer( int nTimerID )
	{
		NeedParentUpdate();
	}

	void CSliderTabLayoutUI::SetPos(RECT rc)
	{
		CTabLayoutUI::SetPos(rc);

		RECT rcInset = GetInset();
		rc.left += rcInset.left;
		rc.top += rcInset.top;
		rc.right -= rcInset.right;
		rc.bottom -= rcInset.bottom;

		if(m_bAnimat)
		{
			if (!m_bAnimating)
			{
				m_bAnimating = true;
				int iStepLen = 0;
				if( !m_bIsVertical )  //横向
				{
					iStepLen = ( rc.right - rc.left ) * m_nPositiveDirection / m_nFrameCount;
					if( m_iCurFrame != m_nFrameCount )
					{
						//当前窗体位置
						::OffsetRect(&m_rcCurPos,iStepLen,0);
						m_pCurPage->SetPos(m_rcCurPos);
						//下一个窗体位置
						::OffsetRect(&m_rcNextPos,iStepLen,0);
						m_pNextPage->SetPos(m_rcNextPos);
					}
					else
					{
						m_pCurPage->SetVisible(false);
						::OffsetRect(&m_rcCurPos,iStepLen,0);
						m_pCurPage->SetPos(m_rcCurPos);
						m_pNextPage->SetPos(rc);
					}
				}
				else //竖向
				{
					iStepLen = ( rc.bottom - rc.top ) * m_nPositiveDirection / m_nFrameCount;
					if( m_iCurFrame != m_nFrameCount )
					{
						//当前窗体位置
						::OffsetRect(&m_rcCurPos,0,iStepLen);	
						m_pCurPage->SetPos(m_rcCurPos);
						//下一个窗体位置
						::OffsetRect(&m_rcNextPos,0,iStepLen);	
						m_pNextPage->SetPos(m_rcNextPos);		
					}
					else
					{
						m_pCurPage->SetVisible(false);
						::OffsetRect(&m_rcCurPos,0,iStepLen);	
						m_pCurPage->SetPos(m_rcCurPos);
						m_pNextPage->SetPos(rc);
					}
				}
				if( m_iCurFrame == m_nFrameCount )
				{
					m_iCurFrame = 0;
					m_bAnimat = false;
					m_pManager->KillTimer( this, TIMER_ANIMATION_ID );
				}
				m_iCurFrame ++;
				m_bAnimating = false;
			}
		}
		else
		{
			for (int it = 0; it < GetCount(); it++) {
				CControlUI* pControl = GetItemAt(it);
				if (!pControl->IsVisible()) continue;
				if (pControl->IsFloat()) {
					SetFloatPos(it);
					continue;
				}

				if (it != GetCurSel()) continue;

				RECT rcPadding = pControl->GetPadding();
				rc.left += rcPadding.left;
				rc.top += rcPadding.top;
				rc.right -= rcPadding.right;
				rc.bottom -= rcPadding.bottom;

				SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };

				SIZE sz = pControl->EstimateSize(szAvailable);
				if (sz.cx == 0) {
					sz.cx = MAX(0, szAvailable.cx);
				}
				if (sz.cx < pControl->GetMinWidth()) sz.cx = pControl->GetMinWidth();
				if (sz.cx > pControl->GetMaxWidth()) sz.cx = pControl->GetMaxWidth();

				if (sz.cy == 0) {
					sz.cy = MAX(0, szAvailable.cy);
				}
				if (sz.cy < pControl->GetMinHeight()) sz.cy = pControl->GetMinHeight();
				if (sz.cy > pControl->GetMaxHeight()) sz.cy = pControl->GetMaxHeight();

				RECT rcCtrl = {rc.left, rc.top, rc.left + sz.cx, rc.top + sz.cy};
				pControl->SetPos(rcCtrl);
			}
		}
	}

	void CSliderTabLayoutUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcsicmp(pstrName, _T("direction")) == 0 && _tcsicmp( pstrValue, _T("vertical")) == 0 ) m_bIsVertical = true; // pstrValue = "vertical" or "horizontal"
		else if( _tcsicmp(pstrName, _T("ellapse")) == 0)m_nEllapse = _ttoi(pstrValue);
		else if( _tcsicmp(pstrName, _T("framecount")) == 0)m_nFrameCount = _ttoi(pstrValue);
		else if( _tcsicmp(pstrName, _T("scrollwheel")) == 0 && _tcsicmp( pstrValue, _T("true")) == 0)m_bScrollWheel = true;
		return CTabLayoutUI::SetAttribute(pstrName, pstrValue);
	}

	void CSliderTabLayoutUI::LineUp()
	{
		if (m_iCurSel > 0)
		{
			SelectItem(m_iCurSel - 1);
		}
	}

	void CSliderTabLayoutUI::LineDown()
	{
		if (m_iCurSel < GetCount())
		{
			SelectItem(m_iCurSel + 1);
		}
	}

} // namespace DuiLib