#ifndef __UIDATETIME_H__
#define __UIDATETIME_H__

#pragma once

//给该控件添加一个属性dtstyle

#define  DTUI_TIMEFORMAT	_T("timeformat")
namespace DuiLib
{
	class CDateTimeWnd;

	/// 时间选择控件
	class UILIB_API CDateTimeUI : public CLabelUI
	{
		friend class CDateTimeWnd;
	public:
		CDateTimeUI();
		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		SYSTEMTIME& GetTime();
		void SetTime(SYSTEMTIME* pst);

		void SetReadOnly(bool bReadOnly);
		bool IsReadOnly() const;

		void SetFormat(LPCTSTR lpszFormat);
		LPCTSTR GetFormat();

		void UpdateText();

		void DoEvent(TEventUI& event);

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		UINT GetDptStyle(){return m_uDptStyle;}

		UINT GetControlFlags() const;
	protected:
		SYSTEMTIME m_sysTime;
		//int        m_nDTUpdateFlag;
		bool       m_bReadOnly;
		UINT		m_uDptStyle;
		CDuiString	m_strFormat;

		CDateTimeWnd* m_pWindow;
	};
}
#endif // __UIDATETIME_H__