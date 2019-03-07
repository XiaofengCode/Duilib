#include "duipub.h"
#include "UIProgress.h"

namespace DuiLib
{
	CProgressUI::CProgressUI() : m_nMin(0), m_nMax(100), m_nValue(0), m_bStretchForeImage(true),
		m_Type(TypeNormol), m_nCirFinishedWidth(0), m_nCirLastWidth(0), m_nCirSpace(0), m_nStartAngle(-90),m_nSweepAngle(0), m_bButton(false)
	{
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
		return __super::GetInterface(pstrName);
	}

	bool CProgressUI::IsHorizontal() const
	{
		return m_Type == TypeHorizontal;
	}

// 	void CProgressUI::SetHorizontal(bool bHorizontal)
// 	{
// 		if( m_Type == TypeHorizontal == bHorizontal ) return;
// 
// 		m_bHorizontal = bHorizontal;
// 		Invalidate();
// 	}

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

	int CProgressUI::GetCircularFinishedWidth() const
	{
		return m_nCirFinishedWidth;
	}

	void CProgressUI::SetCircularFinishedWidth(int nValue)
	{
		m_nCirFinishedWidth = nValue;
	}

	int CProgressUI::GetCircularLastWidth() const
	{
		return m_nCirLastWidth;
	}

	void CProgressUI::SetCircularLastWidth(int nValue)
	{
		m_nCirLastWidth = nValue;
	}

	int CProgressUI::GetCircularSpace() const
	{
		return m_nCirSpace;
	}

	void CProgressUI::SetCircularSpace(int nValue)
	{
		m_nCirSpace = nValue;
	}

	int CProgressUI::GetCircularStartAngle() const
	{
		return m_nStartAngle;
	}

	void CProgressUI::SetCircularStartAngle(int nValue)
	{
		if (m_nStartAngle < -360 || m_nStartAngle > 360)
		{
			return;
		}
		m_nStartAngle = nValue;
	}

	int CProgressUI::GetCircularSweepAngle() const
	{
		return m_nSweepAngle;
	}

	void CProgressUI::SetCircularSweepAngle(int nValue)
	{
		if (m_nSweepAngle < -360 || m_nSweepAngle > 360)
		{
			return;
		}
		m_nSweepAngle = nValue;
	}

	int CProgressUI::IsButton() const
	{
		return m_bButton;
	}

	void CProgressUI::SetButton(bool bValue)
	{
		m_bButton = bValue;
	}

	DuiLib::CProgressUI::ProgressType CProgressUI::GetType() const
	{
		return m_Type;
	}

	void CProgressUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcsicmp(pstrName, _T("hor")) == 0 )
		{
			if(_tcsicmp(pstrValue, _T("true")) == 0)
			{
				SetType(TypeHorizontal);
			}
			else
			{
				SetType(TypeVertical);
			}
		}
		else if( _tcsicmp(pstrName, _T("min")) == 0 ) SetMinValue(_ttoi(pstrValue));
		else if( _tcsicmp(pstrName, _T("max")) == 0 ) SetMaxValue(_ttoi(pstrValue));
		else if( _tcsicmp(pstrName, _T("value")) == 0 ) SetValue(_ttoi(pstrValue));
		else if( _tcsicmp(pstrName, _T("isstretchfore"))==0) SetStretchForeImage(_tcsicmp(pstrValue, _T("true")) == 0? true : false);
		else if( _tcsicmp(pstrName, _T("type"))==0)
		{
			if (_tcsicmp(pstrValue, _T("1")) == 0 ||  _tcsicmp(pstrValue, _T("Ver")) == 0 || _tcsicmp(pstrValue, _T("Vertical")) == 0)
			{
				SetType(TypeVertical);
			}
			else if (_tcsicmp(pstrValue, _T("2")) == 0 ||  _tcsicmp(pstrValue, _T("Cir")) == 0 || _tcsicmp(pstrValue, _T("Circular")) == 0)
			{
				SetType(TypeCircular);
			}
			else
			{
				SetType(TypeHorizontal);
			}
		}
		else if( _tcsicmp(pstrName, _T("CirFinishedWidth")) == 0 ) SetCircularFinishedWidth(_ttoi(pstrValue));
		else if( _tcsicmp(pstrName, _T("CirLastWidth")) == 0 ) SetCircularLastWidth(_ttoi(pstrValue));
		else if( _tcsicmp(pstrName, _T("CirSpace")) == 0 ) SetCircularSpace(_ttoi(pstrValue));
		else if( _tcsicmp(pstrName, _T("StartAngle")) == 0 ) SetCircularStartAngle(_ttoi(pstrValue));
		else if( _tcsicmp(pstrName, _T("SweepAngle")) == 0 ) SetCircularSweepAngle(_ttoi(pstrValue));
		else if( _tcsicmp(pstrName, _T("Button")) == 0 ) SetButton(_tcsicmp(pstrValue, _T("true")) == 0);
		else __super::SetAttribute(pstrName, pstrValue);
	}

	void CProgressUI::PaintBkColor(HDC hDC)
	{
		
	}

	void CProgressUI::PaintForeColor(HDC hDC)
	{
		if( m_nMax <= m_nMin ) m_nMax = m_nMin + 1;
		if( m_nValue > m_nMax ) m_nValue = m_nMax;
		if( m_nValue < m_nMin ) m_nValue = m_nMin;
		DWORD dwColor = GetStatusColor(DUI_ATTR_POS_FORE DUI_ATTR_COLOR);
		if (GetType() != TypeCircular)
		{
			RECT rc = {0};
			if( IsHorizontal() ) {
				rc.right = (LONGLONG)(m_nValue - m_nMin) * (m_rcItem.right - m_rcItem.left) / (m_nMax - m_nMin);
				rc.bottom = m_rcItem.bottom - m_rcItem.top;
			}
			else {
				rc.top = (LONGLONG)(m_rcItem.bottom - m_rcItem.top) * (m_nMax - m_nValue) / (m_nMax - m_nMin);
				rc.right = m_rcItem.right - m_rcItem.left;
				rc.bottom = m_rcItem.bottom - m_rcItem.top;
			}
			rc.left += m_rcItem.left;
			rc.right += m_rcItem.left;
			rc.top += m_rcItem.top;
			rc.bottom += m_rcItem.top;
			CRenderEngine::DrawColor(hDC, rc, dwColor);
		}
		else
		{
			//if( !::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem) ) return;
			Gdiplus::Graphics g(hDC);
			g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);//抗锯齿 
			CDuiRect rcItem(m_rcItem.left + m_rcPadding.left,
				m_rcItem.top + m_rcPadding.top,
				m_rcItem.right + m_rcPadding.right,
				m_rcItem.bottom + m_rcPadding.bottom);
			//TODO:如果有边框就先绘制边框
			//因为画笔是居中画的，因此要先缩小矩形，缩小圆环宽度的一半
			int nWidth = GetCircularFinishedWidth();
			if (!nWidth)
			{
				nWidth = rcItem.GetHeight()/5;
			}
			CDuiRect rcValue(rcItem);
			rcValue.Deflate(nWidth/2, nWidth/2);
			Gdiplus::Pen pen(dwColor);
			pen.SetWidth(nWidth);
			int nSweepValue = GetValue() * GetCircularSweepAngle() / GetMaxValue();
			//绘制已完成部分
			double nStartAngle = GetCircularStartAngle() + GetCircularSpace();
			double nSweepAngle = nSweepValue - GetCircularSpace()*0.5f;
			if (nSweepAngle <= 0)
			{
				nSweepAngle = 2;
			}
			g.DrawArc(&pen, rcValue.left, rcValue.top, rcValue.GetWidth(), rcValue.GetHeight(), nStartAngle, nSweepAngle);

			nWidth = GetCircularLastWidth();
			if (!nWidth)
			{
				nWidth = rcItem.GetHeight()/5;
			}
			CDuiRect rcLast(rcItem);
			rcLast.Deflate(nWidth/2, nWidth/2);
			//rcLast.Deflate(rcItem.GetWidth()*GetCircularSpace()/360, rcItem.GetWidth()*GetCircularSpace()/360);
			//绘制剩余部分
			pen.SetColor(GetBkColor());
			pen.SetWidth(nWidth);
			nStartAngle = GetCircularStartAngle() + GetCircularSpace()*1.5f + nSweepValue - 1;
			nSweepAngle = GetCircularSweepAngle() - nSweepAngle - GetCircularSpace() * 2 + 2;
			if (nSweepAngle <= 0)
			{
				nSweepAngle = 2;
			}
			g.DrawArc(&pen, rcLast.left, rcLast.top, rcLast.GetWidth(), rcLast.GetHeight(), nStartAngle, nSweepAngle);
		}
	}

	void CProgressUI::PaintNormalImage(HDC hDC)
	{
		if( m_nMax <= m_nMin ) m_nMax = m_nMin + 1;
		if( m_nValue > m_nMax ) m_nValue = m_nMax;
		if( m_nValue < m_nMin ) m_nValue = m_nMin;

		if (GetType() != TypeCircular)
		{
			RECT rc = {0};
			if( IsHorizontal() ) {
				rc.right = (LONGLONG)(m_nValue - m_nMin) * (m_rcItem.right - m_rcItem.left) / (m_nMax - m_nMin);
				rc.bottom = m_rcItem.bottom - m_rcItem.top;
			}
			else {
				rc.top = (LONGLONG)(m_rcItem.bottom - m_rcItem.top) * (m_nMax - m_nValue) / (m_nMax - m_nMin);
				rc.right = m_rcItem.right - m_rcItem.left;
				rc.bottom = m_rcItem.bottom - m_rcItem.top;
			}
			CDuiString sForeImage = GetAttribute(DUI_ATTR_POS_FORE DUI_ATTR_IMAGE);
			if( !sForeImage.IsEmpty() ) {
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

				if( !DrawImage(hDC, (LPCTSTR)sForeImage, (LPCTSTR)m_sForeImageModify) ) sForeImage.Empty();
				else return;
			}
		}
		else
		{
			__super::PaintNormalImage(hDC);
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

	void CProgressUI::SetType(ProgressType type)
	{
		m_Type = type;
		Invalidate();
	}

	void CProgressUI::DoEvent(TEventUI& event)
	{
		if (m_bButton)
		{
			CButtonUI::DoEvent(event);
		}
		else
		{
			CLabelUI::DoEvent(event);
		}
	}

}
