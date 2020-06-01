#ifndef __UIDATETIME_H__
#define __UIDATETIME_H__

#pragma once

//���ÿؼ����һ������dtstyle

#define  DTUI_TIMEFORMAT	_T("timeformat")
namespace DuiLib
{
	class CDateTimeWnd;

	/// ʱ��ѡ��ؼ�
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