#include "duipub.h"
#include "UIOption.h"

namespace DuiLib
{
	COptionUI::COptionUI() : m_bSelected(false)
	{
	}

	COptionUI::~COptionUI()
	{
		if( !m_sGroupName.IsEmpty() && m_pManager ) m_pManager->RemoveOptionGroup(m_sGroupName, this);
	}

	LPCTSTR COptionUI::GetClass() const
	{
		return _T("OptionUI");
	}

	LPVOID COptionUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_OPTION) == 0 ) return static_cast<COptionUI*>(this);
		return CButtonUI::GetInterface(pstrName);
	}

	void COptionUI::SetManager(CPaintManagerUI* pManager, CControlUI* pParent, bool bInit)
	{
		CButtonUI::SetManager(pManager, pParent, bInit);
		if( bInit && !m_sGroupName.IsEmpty() ) {
			if (m_pManager) m_pManager->AddOptionGroup(m_sGroupName, this);
		}
	}

	LPCTSTR COptionUI::GetGroup() const
	{
		return m_sGroupName;
	}

	void COptionUI::SetGroup(LPCTSTR pStrGroupName)
	{
		if( pStrGroupName == NULL ) {
			if( m_sGroupName.IsEmpty() ) return;
			m_sGroupName.Empty();
		}
		else {
			if( m_sGroupName == pStrGroupName ) return;
			if (!m_sGroupName.IsEmpty() && m_pManager) m_pManager->RemoveOptionGroup(m_sGroupName, this);
			m_sGroupName = pStrGroupName;
		}

		if( !m_sGroupName.IsEmpty() ) {
			if (m_pManager) m_pManager->AddOptionGroup(m_sGroupName, this);
		}
		else {
			if (m_pManager) m_pManager->RemoveOptionGroup(m_sGroupName, this);
		}

		Selected(m_bSelected);
	}

	bool COptionUI::IsSelected() const
	{
		return m_bSelected;
	}

	void COptionUI::Selected(bool bSelected)
	{
//		if( m_bSelected == bSelected ) return;
		m_bSelected = bSelected;
		if( m_bSelected ) m_uButtonState |= UISTATE_SELECTED;
		else m_uButtonState &= ~UISTATE_SELECTED;

		if( m_pManager != NULL ) 
		{
			if( !m_sGroupName.IsEmpty() ) 
			{
				if( m_bSelected ) 
				{
					CStdPtrArray* aOptionGroup = m_pManager->GetOptionGroup(m_sGroupName);
					for( int i = 0; i < aOptionGroup->GetSize(); i++ ) 
					{
						COptionUI* pControl = static_cast<COptionUI*>(aOptionGroup->GetAt(i));
						if( pControl != this ) 
						{
							pControl->Selected(false);
						}
					}
					m_pManager->SendNotify(this, DUI_MSGTYPE_SELECTCHANGED);
				}
				else
				{
					m_pManager->SendNotify(this, DUI_MSGTYPE_UNSELECTED);
				}
			}
			else 
			{
				m_pManager->SendNotify(this, DUI_MSGTYPE_SELECTCHANGED);
			}
		}

		Invalidate();
	}

	bool COptionUI::Activate()
	{
		if( !CButtonUI::Activate() ) return false;
		if( !m_sGroupName.IsEmpty() ) Selected(true);
		else Selected(!m_bSelected);

		return true;
	}

	void COptionUI::SetEnabled(bool bEnable)
	{
		CButtonUI::SetEnabled(bEnable);
		if( !IsEnabled() ) {
			if( m_bSelected ) m_uButtonState = UISTATE_SELECTED;
			else m_uButtonState = 0;
		}
	}

	LPCTSTR COptionUI::GetSelectedImage()
	{
		return m_sSelectedImage;
	}

	void COptionUI::SetSelectedImage(LPCTSTR pStrImage)
	{
		m_sSelectedImage = pStrImage;
		Invalidate();
	}

	LPCTSTR COptionUI::GetSelectedHotImage()
	{
		return m_sSelectedHotImage;
	}

	void COptionUI::SetSelectedHotImage( LPCTSTR pStrImage )
	{
		m_sSelectedHotImage = pStrImage;
		Invalidate();
	}

	LPCTSTR COptionUI::GetSelectedPushedImage()
	{
		return m_sSelectedPushedImage;
	}

	void COptionUI::SetSelectedPushedImage(LPCTSTR pStrImage)
	{
		m_sSelectedPushedImage = pStrImage;
		Invalidate();
	}

	void COptionUI::SetSelectedTextColor(DWORD dwTextColor)
	{
		m_attrs.SetAttribute(DUI_ATTR_STATUS_SELECTED DUI_ATTR_TEXT DUI_ATTR_COLOR, dwTextColor);
	}

	DWORD COptionUI::GetSelectedTextColor()
	{
		return m_attrs.GetColor(DUI_ATTR_STATUS_SELECTED DUI_ATTR_TEXT DUI_ATTR_COLOR);
	}

	void COptionUI::SetSelectedBkColor( DWORD dwBkColor )
	{
		m_attrs.SetAttribute(DUI_ATTR_STATUS_SELECTED DUI_ATTR_POS_BK DUI_ATTR_COLOR, dwBkColor);
	}

	DWORD COptionUI::GetSelectBkColor()
	{
		return m_attrs.GetColor(DUI_ATTR_STATUS_SELECTED DUI_ATTR_POS_BK DUI_ATTR_COLOR);
	}

	LPCTSTR COptionUI::GetForeImage()
	{
		return m_sForeImage;
	}

	void COptionUI::SetForeImage(LPCTSTR pStrImage)
	{
		m_sForeImage = pStrImage;
		Invalidate();
	}

	LPCTSTR COptionUI::GetSelectedForedImage()
	{
		return m_sSelectedForeImage;
	}

	void COptionUI::SetSelectedForedImage(LPCTSTR pStrImage)
	{
		m_sSelectedForeImage = pStrImage;
		Invalidate();
	}

	SIZE COptionUI::EstimateSize(SIZE szAvailable)
	{
		if( GetFixedHeight() == 0 )
			return CDuiSize(GetFixedWidth(), m_pManager->GetFontInfo(GetFont())->tm.tmHeight + 8);
		return CButtonUI::EstimateSize(szAvailable);
	}

	void COptionUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcsicmp(pstrName, _T("group")) == 0 ) SetGroup(pstrValue);
		else if( _tcsicmp(pstrName, _T("selected")) == 0 ) Selected(_tcsicmp(pstrValue, _T("true")) == 0);
		else if( _tcsicmp(pstrName, _T("selectedimage")) == 0 ) SetSelectedImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("selectedhotimage")) == 0 ) SetSelectedHotImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("selectedpushedimage")) == 0 ) SetSelectedPushedImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("foreimage")) == 0 ) SetForeImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("selectedforeimage")) == 0 ) SetSelectedForedImage(pstrValue);
		else if( _tcsicmp(pstrName, _T("selectedbkcolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetSelectedBkColor(clrColor);
		}
		else if( _tcsicmp(pstrName, _T("selectedtextcolor")) == 0 ) {
			if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetSelectedTextColor(clrColor);
		}
		else CButtonUI::SetAttribute(pstrName, pstrValue);
	}

	void COptionUI::PaintStatusImage(HDC hDC)
	{
		if( (m_uButtonState & UISTATE_PUSHED) != 0 && IsSelected() && !m_sSelectedPushedImage.IsEmpty()) {
			if( !DrawImage(hDC, (LPCTSTR)m_sSelectedPushedImage) )
				m_sSelectedPushedImage.Empty();
			goto Label_ForeImage;
		}
		else if( (m_uButtonState & UISTATE_HOT) != 0 && IsSelected() && !m_sSelectedHotImage.IsEmpty()) {
			if( !DrawImage(hDC, (LPCTSTR)m_sSelectedHotImage) )
				m_sSelectedHotImage.Empty();
			goto Label_ForeImage;
		}
		else if( (m_uButtonState & UISTATE_SELECTED) != 0 ) {
			if(GetSelectBkColor() != 0) {
				CRenderEngine::DrawColor(hDC, m_rcPaint, GetAdjustColor(GetSelectBkColor()));
				//goto Label_ForeImage;
			}	
			if( !m_sSelectedImage.IsEmpty() ) {
				if( !DrawImage(hDC, (LPCTSTR)m_sSelectedImage) ) m_sSelectedImage.Empty();
				//goto Label_ForeImage;
			}
			goto Label_ForeImage;
		}

		CButtonUI::PaintStatusImage(hDC);

Label_ForeImage:
		if ( IsSelected() && !m_sSelectedForeImage.IsEmpty())
		{
			if( !DrawImage(hDC, (LPCTSTR)m_sSelectedForeImage) ) m_sSelectedForeImage.Empty();
		}
		else if(  !m_sForeImage.IsEmpty() ) 
		{
			if( !DrawImage(hDC, (LPCTSTR)m_sForeImage) ) m_sForeImage.Empty();
		}

	}

	void COptionUI::PaintText(HDC hDC)
	{
		if( GetTextColor() == 0 )
			m_attrs.SetAttribute(DUI_ATTR_TEXT DUI_ATTR_COLOR, m_pManager->GetDefaultFontColor());
		if( GetDisabledTextColor() == 0 )
			m_attrs.SetAttribute(DUI_ATTR_STATUS_DISABLED DUI_ATTR_TEXT DUI_ATTR_COLOR, m_pManager->GetDefaultDisabledColor());
		if( (m_uButtonState & UISTATE_SELECTED) != 0 )
		{
			DWORD dwTextColor = GetTextColor();
			if( GetSelectedTextColor() != 0 ) dwTextColor = GetSelectedTextColor();
			if (!IsEnabled())
			{
				dwTextColor = GetDisabledTextColor();
			}


			if( m_sText.IsEmpty() ) return;
			int nLinks = 0;
			RECT rc = m_rcItem;
			rc.left += m_rcTextPadding.left;
			rc.right -= m_rcTextPadding.right;
			rc.top += m_rcTextPadding.top;
			rc.bottom -= m_rcTextPadding.bottom;

			if( m_bShowHtml )
				CRenderEngine::DrawHtmlText(hDC, m_pManager, rc, m_sText, dwTextColor, \
				NULL, NULL, nLinks, m_uTextStyle);
			else
				CRenderEngine::DrawText(hDC, m_pManager, rc, m_sText, dwTextColor, \
				m_sFont, m_uTextStyle);
		}
		else
			CButtonUI::PaintText(hDC);
	}
}