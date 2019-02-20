#ifndef __UIPROGRESS_H__
#define __UIPROGRESS_H__

#pragma once

namespace DuiLib
{
	class UILIB_API CProgressUI : public CButtonUI
	{
	public:
		LBIND_CLASS_DEFINE(CProgressUI, CButtonUI);
		CProgressUI();
		enum ProgressType{
			TypeNormol,
			TypeHorizontal = TypeNormol,
			TypeVertical,
			TypeCircular,
		};

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		bool IsHorizontal() const;
		//void SetHorizontal(bool bHorizontal = true);
		bool IsStretchForeImage();
		void SetStretchForeImage(bool bStretchForeImage = true);
		int GetMinValue() const;
		void SetMinValue(int nMin);
		int GetMaxValue() const;
		void SetMaxValue(int nMax);
		int GetValue() const;
		void SetValue(int nValue);
		LPCTSTR GetForeImage() const;
		void SetForeImage(LPCTSTR pStrImage);
		void SetForeColor(DWORD dwTextColor);
		DWORD GetForeColor() const;
		int GetCircularFinishedWidth() const;
		void SetCircularFinishedWidth(int nValue);
		int GetCircularLastWidth() const;
		void SetCircularLastWidth(int nValue);
		int GetCircularSpace() const;
		void SetCircularSpace(int nValue);
		int GetCircularStartAngle() const;
		void SetCircularStartAngle(int nValue);
		int GetCircularSweepAngle() const;
		void SetCircularSweepAngle(int nValue);
		int IsButton() const;
		void SetButton(bool bValue);

		ProgressType GetType() const;
		void SetType(ProgressType type);

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void PaintStatusImage(HDC hDC);

		void DoPaint(HDC hDC, const RECT& rcPaint);
		void DoEvent(TEventUI& event);

	protected:
		//bool m_bHorizontal;
		bool m_bStretchForeImage;
		int m_nMax;
		int m_nMin;
		int m_nValue;
		DWORD		m_dwForeColor;

		CDuiString m_sForeImage;
		CDuiString m_sForeImageModify;
		ProgressType m_Type;

		//圆环形状属性
		int m_nCirFinishedWidth;	// 已完成圆环宽度
		int m_nCirLastWidth;	// 剩余圆环宽度
		int m_nCirSpace;	// 已完成和未完成的间距
		int m_nStartAngle;	// 起始角度
		int m_nSweepAngle;	// 角度大小
		bool m_bButton;
	};

} // namespace DuiLib

#endif // __UIPROGRESS_H__
