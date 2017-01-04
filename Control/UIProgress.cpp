#include "stdafx.h"
#include "UIProgress.h"

namespace DuiLib
{
	CProgressUI::CProgressUI() : m_bHorizontal(true), m_nMin(0), m_nMax(100), m_nValue(0), m_bStretchForeImage(true)
	{
		m_dwForeColor = 0;
		m_uTextStyle = DT_SINGLELINE | DT_CENTER;
		SetFixedHeight(12);
	}

	LPCTSTR CProgressUI::GetClass() const
	{
		return _T("ProgressUI");
	}

	LPVOID CProgressUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_PROGRESS) == 0 ) return static_cast<CProgressUI*>(this);
		return CLabelUI::GetInterface(pstrName);
	}

	bool CProgressUI::IsHorizontal()
	{
		return m_bHorizontal;
	}

	void CProgressUI::SetHorizontal(bool bHorizontal)
	{
		if( m_bHorizontal == bHorizontal ) return;

		m_bHorizontal = bHorizontal;
		Invalidate();
	}

	int CProgressUI::GetMinValue() const
	{
		return m_nMin;
	}

	void CProgressUI::SetMinValue(int nMin)
	{
		m_nMin = nMin;
		Invalidate();
	}

	int CProgressUI::GetMaxValue() const
	{
		return m_nMax;
	}

	void CProgressUI::SetMaxValue(int nMax)
	{
		m_nMax = nMax;
		Invalidate();
	}

	int CProgressUI::GetValue() const
	{
		return m_nValue;
	}

	void CProgressUI::SetValue(int nValue)
	{
		if(nValue == m_nValue || nValue<m_nMin || nValue > m_nMax)
		{
			return;
		}
		m_nValue = nValue;
		Invalidate();
	}

	LPCTSTR CProgressUI::GetForeImage() const
	{
		return m_sForeImage;
	}

	void CProgressUI::SetForeImage(LPCTSTR pStrImage)
	{
		if( m_sForeImage == pStrImage ) return;

		m_sForeImage = pStrImage;
		Invalidate();
	}

	void CProgressUI::SetForeColor(DWORD dwTextColor)
	{
		m_dwForeColor = dwTextColor;
		Invalidate();
	}

	DWORD CProgressUI::GetForeColor() const
	{
		return m_dwForeColor;
	}

	void CProgressUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcsicmp(pstrName, _T("foreimage")) == 0 ) SetForeImage(pstrValue);
		else if (_tcsicmp(pstrName, _T("forecolor")) == 0)
		{
			while (*pstrValue > _T('\0') && *pstrValue <= _T(' ')) pstrValue = ::CharNext(pstrValue);
			if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
			LPTSTR pstr = NULL;
			DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
			SetForeColor(clrColor);
		}
		else if( _tcsicmp(pstrName, _T("hor")) == 0 ) SetHorizontal(_tcsicmp(pstrValue, _T("true")) == 0);
		else if( _tcsicmp(pstrName, _T("min")) == 0 ) SetMinValue(_ttoi(pstrValue));
		else if( _tcsicmp(pstrName, _T("max")) == 0 ) SetMaxValue(_ttoi(pstrValue));
		else if( _tcsicmp(pstrName, _T("value")) == 0 ) SetValue(_ttoi(pstrValue));
		else if( _tcsicmp(pstrName, _T("isstretchfore"))==0) SetStretchForeImage(_tcsicmp(pstrValue, _T("true")) == 0? true : false);
		else CLabelUI::SetAttribute(pstrName, pstrValue);
	}

	void CProgressUI::PaintStatusImage(HDC hDC)
	{
		if( m_nMax <= m_nMin ) m_nMax = m_nMin + 1;
		if( m_nValue > m_nMax ) m_nValue = m_nMax;
		if( m_nValue < m_nMin ) m_nValue = m_nMin;

		RECT rc = {0};
		if( m_bHorizontal ) {
			rc.right = (LONGLONG)(m_nValue - m_nMin) * (m_rcItem.right - m_rcItem.left) / (m_nMax - m_nMin);
			rc.bottom = m_rcItem.bottom - m_rcItem.top;
		}
		else {
			rc.top = (LONGLONG)(m_rcItem.bottom - m_rcItem.top) * (m_nMax - m_nValue) / (m_nMax - m_nMin);
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
		else
		{
			rc.left += m_rcItem.left;
			rc.right += m_rcItem.left;
			rc.top += m_rcItem.top;
			rc.bottom += m_rcItem.top;
			CRenderEngine::DrawColor(hDC, rc, m_dwForeColor);
		}
	}

	bool CProgressUI::IsStretchForeImage()
	{
		return m_bStretchForeImage;
	}

	void CProgressUI::SetStretchForeImage( bool bStretchForeImage /*= true*/ )
	{
		if (m_bStretchForeImage==bStretchForeImage)		return;
		m_bStretchForeImage=bStretchForeImage;
		Invalidate();
	}
}
