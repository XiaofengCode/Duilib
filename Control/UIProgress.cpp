#include "duipub.h"
#include "UIProgress.h"

namespace DuiLib
{
	CProgressUI::CProgressUI()
		: m_nMin(0)
		, m_nMax(100)
		, m_nValue(0)
		, m_bStretchForeImage(true)
		, m_Type(TypeNormol)
		, m_nCirFinishedWidth(0)
		, m_nCirFinishedWidthPercent(0)
		, m_nCirLastWidth(0)
		, m_nCirLastWidthPercent(0)
		, m_nCirSpace(0)
		, m_nStartAngle(-90)
		, m_nSweepAngle(0)
		, m_bButton(false)
	{
		m_uTextStyle = DT_SINGLELINE | DT_CENTER;
		//SetFixedHeight(12);
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
		double S = 1.0;
		if (m_pManager)
		{
			S = m_pManager->GetDpiScale();
		}
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
		else if( _tcsicmp(pstrName, _T("CirFinishedWidth")) == 0 )
		{
			if (_tcsrchr(pstrValue, '%'))
			{
				m_nCirFinishedWidthPercent = _ttoi(pstrValue);
			}
			else
			{
				SetCircularFinishedWidth(_ttoi(pstrValue) * S);
			}
		}
		else if (_tcsicmp(pstrName, _T("CirLastWidth")) == 0)
		{
			if (_tcsrchr(pstrValue, '%'))
			{
				m_nCirLastWidthPercent = _ttoi(pstrValue);
			}
			else
			{
				SetCircularLastWidth(_ttoi(pstrValue) * S);
			}
		}
		else if( _tcsicmp(pstrName, _T("CirSpace")) == 0 ) SetCircularSpace(_ttoi(pstrValue) * S);
		else if( _tcsicmp(pstrName, _T("StartAngle")) == 0 ) SetCircularStartAngle(_ttoi(pstrValue));
		else if( _tcsicmp(pstrName, _T("SweepAngle")) == 0 ) SetCircularSweepAngle(_ttoi(pstrValue));
		else if( _tcsicmp(pstrName, _T("Button")) == 0 ) SetButton(_tcsicmp(pstrValue, _T("true")) == 0);
		else __super::SetAttribute(pstrName, pstrValue);
	}

	void CProgressUI::PaintBkColor(HDC hDC)
	{
		if (GetType() != TypeCircular)
		{
			__super::PaintBkColor(hDC);
		}
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
			using namespace Gdiplus;
			Gdiplus::Pen pen(dwColor);
			Gdiplus::Graphics g(hDC);
			g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);//抗锯齿 
			RECT rcPadding = GetStatusRect(DUI_ATTR_PADDING);
			CDuiRect rcItem(m_rcItem.left + rcPadding.left,
							m_rcItem.top + rcPadding.top,
							m_rcItem.right - rcPadding.right - 1,
							m_rcItem.bottom - rcPadding.bottom - 1);
			if (rcItem.GetWidth() > rcItem.GetHeight())
			{
				int nDelta = rcItem.GetWidth() - rcItem.GetHeight();
				rcItem.left += nDelta/2;
				rcItem.right = rcItem.left + rcItem.GetHeight();
			}
			else
			{
				int nDelta = rcItem.GetHeight() - rcItem.GetWidth();
				rcItem.top += nDelta/2;
				rcItem.bottom = rcItem.top + rcItem.GetWidth();
			}

			int nLastWidth;
			if (m_nCirLastWidthPercent)
			{
				nLastWidth = rcItem.GetWidth() * m_nCirLastWidthPercent / 100;
			}
			else
			{
				nLastWidth = GetCircularLastWidth();
			}

			int nFinishWidth;
			if (m_nCirFinishedWidthPercent)
			{
				nFinishWidth = rcItem.GetWidth() * m_nCirFinishedWidthPercent / 100;
			}
			else
			{
				nFinishWidth = GetCircularFinishedWidth();
			}
			double dFinishRatio = GetValue() * 1.0 / GetMaxValue();
			int nSweepValue = (int)(dFinishRatio * GetCircularSweepAngle());
			const static double PI = 3.1415926535898;
			double a = GetCircularSpace();					//外圆弦长
			double r2 = rcItem.GetHeight();					//外圆半径
			double r1 = rcItem.GetHeight() - nFinishWidth;	//内圆半径
			double x = a * r1 / (2 * r2);					//内圆半弦长
			double b = a * (r2 - r1) / r2;					//内圆弦需要延长到达平行的长度
			double tan2 = (b + x) / sqrt(r1 * r1 - x * x);	//空白部分所占角的一半 + 延长角度 的tan值
			double angel2 = atan(tan2) * 180 / PI;			//空白部分所占角的一半 + 延长角度
			double sin1 = a / 2 / r2;						//空白部分所占角的一半的sin值
			double angel1 = asin(sin1) * 180 / PI;			//空白部分所占角度的一半
			angel2 -= angel1;								//延长角度
			//angel1 *= 2;
			double dAngelIn = (angel2 + angel1) * 2;
			double dAngelInFinish = dAngelIn * dFinishRatio;
			double dAngelInLast = dAngelIn - dAngelInFinish;

			double dAngelOut = angel1 * 2;
			double dAngelOutFinish = dAngelOut * dFinishRatio;
			double dAngelOutLast = dAngelIn - dAngelOutFinish;
			if (nSweepValue < GetCircularSweepAngle())
			{
				GraphicsPath pathLast;
				//int nSweepValue = GetValue() * GetCircularSweepAngle() / GetMaxValue();

				CDuiRect rcValue(rcItem);
				rcValue.Deflate(nLastWidth, nLastWidth);
				if (GetValue() == 0)
				{
					double nStartAngle = GetCircularStartAngle();
					double nSweepAngle = GetCircularSweepAngle();
					pathLast.AddArc(rcItem.left, rcItem.top, rcItem.GetWidth(), rcItem.GetHeight(), nStartAngle, nSweepAngle);
					pathLast.AddArc(rcValue.left, rcValue.top, rcValue.GetWidth(), rcValue.GetHeight(), nStartAngle + nSweepAngle, -nSweepAngle);
				}
				else
				{
					double nStartAngle = GetCircularStartAngle() + dAngelInLast;
					double nSweepAngle = GetCircularSweepAngle() - nSweepValue - dAngelInLast * 2;
					pathLast.AddArc(rcItem.left, rcItem.top, rcItem.GetWidth(), rcItem.GetHeight(), nStartAngle, nSweepAngle);
					pathLast.AddArc(rcValue.left, rcValue.top, rcValue.GetWidth(), rcValue.GetHeight(), nStartAngle + nSweepAngle - angel2, -(nSweepAngle - 2 * angel2));
				}
				pathLast.CloseFigure();
				DWORD dwBkColor = GetStatusColor(DUI_ATTR_POS_BK DUI_ATTR_COLOR);
				SolidBrush brush(Gdiplus::Color(dwBkColor >> 24, (dwBkColor >> 16) & 0xFF, (dwBkColor >> 8) & 0xFF, dwBkColor & 0xFF));
				g.FillPath(&brush, &pathLast);
			}


			//绘制已完成部分
			if (nSweepValue)
			{
				GraphicsPath pathFinish;
				double nStartAngle = GetCircularStartAngle() - dAngelOutFinish;
				double nSweepAngle = nSweepValue - dAngelOutFinish * 2;
				CDuiRect rcValueIn(rcItem);
				rcValueIn.Deflate(nFinishWidth, nFinishWidth);
				if (GetValue() == GetMaxValue())
				{
					pathFinish.AddArc(rcItem.left, rcItem.top, rcItem.GetWidth(), rcItem.GetHeight(), GetCircularStartAngle(), -GetCircularSweepAngle());
					pathFinish.AddArc(rcValueIn.left, rcValueIn.top, rcValueIn.GetWidth(), rcValueIn.GetHeight(),
									  nStartAngle - GetCircularSweepAngle(), GetCircularSweepAngle());
				}
				else
				{
					double nStartAngle = GetCircularStartAngle() - dAngelOutFinish;
					double nSweepAngle = nSweepValue - dAngelOutFinish * 2;
					pathFinish.AddArc(rcItem.left, rcItem.top, rcItem.GetWidth(), rcItem.GetHeight(), nStartAngle, -nSweepAngle);
					pathFinish.AddArc(rcValueIn.left, rcValueIn.top, rcValueIn.GetWidth(), rcValueIn.GetHeight(),
									  nStartAngle - nSweepAngle + angel2,
									  nSweepAngle - angel2 * 2 );
				}
				pathFinish.CloseFigure();
				SolidBrush brush(Gdiplus::Color(dwColor >> 24, (dwColor >> 16) & 0xFF, (dwColor >> 8) & 0xFF, dwColor & 0xFF));
				g.FillPath(&brush, &pathFinish);
			}
		}
	}

	void CProgressUI::PaintForeImage(HDC hDC)
	{

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
