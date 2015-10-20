#include "stdafx.h"
#include "UIProgressButton.h"

namespace DuiLib
{
	CProgressButtonUI::CProgressButtonUI() : m_bHorizontal(true), m_nMin(0), m_nMax(100), m_nValue(0), m_bStretchForeImage(true)
	{
		m_uTextStyle = DT_SINGLELINE | DT_CENTER;
		SetFixedHeight(12);
	}

	LPCTSTR CProgressButtonUI::GetClass() const
	{
		return _T("ProgressButtonUI");
	}

	LPVOID CProgressButtonUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_PROGRESSBUTTON) == 0 ) return static_cast<CProgressButtonUI*>(this);
		return CLabelUI::GetInterface(pstrName);
	}

	bool CProgressButtonUI::IsHorizontal()
	{
		return m_bHorizontal;
	}

	void CProgressButtonUI::SetHorizontal(bool bHorizontal)
	{
		if( m_bHorizontal == bHorizontal ) return;

		m_bHorizontal = bHorizontal;
		Invalidate();
	}

	int CProgressButtonUI::GetMinValue() const
	{
		return m_nMin;
	}

	void CProgressButtonUI::SetMinValue(int nMin)
	{
		m_nMin = nMin;
		Invalidate();
	}

	int CProgressButtonUI::GetMaxValue() const
	{
		return m_nMax;
	}

	void CProgressButtonUI::SetMaxValue(int nMax)
	{
		m_nMax = nMax;
		Invalidate();
	}

	int CProgressButtonUI::GetValue() const
	{
		return m_nValue;
	}

	void CProgressButtonUI::SetValue(int nValue)
	{
		m_nValue = nValue;
		Invalidate();
	}

	LPCTSTR CProgressButtonUI::GetForeImage() const
	{
		return m_sForeImage;
	}

	void CProgressButtonUI::SetForeImage(LPCTSTR pStrImage)
	{
		if( m_sForeImage == pStrImage ) return;

		m_sForeImage = pStrImage;
		Invalidate();
	}

	void CProgressButtonUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcscmp(pstrName, _T("foreimage")) == 0 ) SetForeImage(pstrValue);
		else if( _tcscmp(pstrName, _T("hor")) == 0 ) SetHorizontal(_tcscmp(pstrValue, _T("true")) == 0);
		else if( _tcscmp(pstrName, _T("min")) == 0 ) SetMinValue(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("max")) == 0 ) SetMaxValue(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("value")) == 0 ) SetValue(_ttoi(pstrValue));
		else if( _tcscmp(pstrName, _T("isstretchfore"))==0) SetStretchForeImage(_tcscmp(pstrValue, _T("true")) == 0? true : false);
		else CButtonUI::SetAttribute(pstrName, pstrValue);
	}

	void CProgressButtonUI::PaintStatusImage(HDC hDC)
	{
		CButtonUI::PaintStatusImage(hDC);
		if( m_nMax <= m_nMin ) m_nMax = m_nMin + 1;
		if( m_nValue > m_nMax ) m_nValue = m_nMax;
		if( m_nValue < m_nMin ) m_nValue = m_nMin;

		RECT rc = {0};
		if( m_bHorizontal ) {
			rc.right = (m_nValue - m_nMin) * (m_rcItem.right - m_rcItem.left) / (m_nMax - m_nMin);
			rc.bottom = m_rcItem.bottom - m_rcItem.top;
		}
		else {
			rc.top = (m_rcItem.bottom - m_rcItem.top) * (m_nMax - m_nValue) / (m_nMax - m_nMin);
			rc.right = m_rcItem.right - m_rcItem.left;
			rc.bottom = m_rcItem.bottom - m_rcItem.top;
		}

		if( !m_sForeImage.IsEmpty() ) {
			m_sForeImageModify.Empty();
			double S = GetScaleDpi();
			if (m_bStretchForeImage)
			{
				m_sForeImageModify.SmallFormat(_T("dest='%d,%d,%d,%d'"), 
					(int)(rc.left / S), (int)(rc.top / S), (int)(rc.right / S), (int)(rc.bottom / S));
			}
			else
			{
				m_sForeImageModify.SmallFormat(_T("dest='%d,%d,%d,%d' source='%d,%d,%d,%d'")
					, (int)(rc.left / S), (int)(rc.top / S), (int)(rc.right / S), (int)(rc.bottom / S)
					, (int)(rc.left / S), (int)(rc.top / S), (int)(rc.right / S), (int)(rc.bottom / S));
			}

			if( !DrawImage(hDC, (LPCTSTR)m_sForeImage, (LPCTSTR)m_sForeImageModify) ) m_sForeImage.Empty();
			else return;
		}
	}

	bool CProgressButtonUI::IsStretchForeImage()
	{
		return m_bStretchForeImage;
	}

	void CProgressButtonUI::SetStretchForeImage( bool bStretchForeImage /*= true*/ )
	{
		if (m_bStretchForeImage==bStretchForeImage)		return;
		m_bStretchForeImage=bStretchForeImage;
		Invalidate();
	}
}
