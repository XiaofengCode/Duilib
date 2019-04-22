#ifndef DuiTrayIcon_h__
#define DuiTrayIcon_h__

#pragma once
#include <ShellAPI.h>
#define WM_DUI_TRAYICON_MESSAGE			(WM_USER + 'TM')
namespace DuiLib
{
	class IDuiTimer;
	class UILIB_API CDuiTrayIconUI : public CControlUI
	{
	public:
		CDuiTrayIconUI(void);
		~CDuiTrayIconUI(void);

	public:
		void CreateTrayIcon( CPaintManagerUI* pManager, UINT _IconIDResource, LPCTSTR _ToolTipText = NULL, UINT _Message = 0);
		void CreateTrayIcon( CPaintManagerUI* pManager, LPCTSTR _IconPath, LPCTSTR _ToolTipText = NULL, UINT _Message =0);
		void DeleteTrayIcon();
		bool SetTooltipText(LPCTSTR _ToolTipText);
		bool SetTooltipText(UINT _IDResource);
		CDuiString GetTooltipText() const;

		bool SetIcon(HICON _Hicon);
		bool SetIcon(LPCTSTR _IconFile);
		bool SetIcon(UINT _IDResource);
		HICON GetIcon() const;
		void SetHideIcon();
		void SetShowIcon();
		void RemoveIcon();
		bool StartTwinkling();
		void StopTwinkling();

		bool Enabled(){return m_bEnabled;};
		bool IsVisible(){return !m_bVisible;};
		bool IsTwinkling(){return m_bTwinkling;};

		void OnTimer(IDuiTimer* pTimer);
		bool ShowMessage(LPCTSTR lpMsg, LPCTSTR lpTitle, UINT uTimeout = 3000);
	private:
		bool m_bEnabled;
		//bool m_bVisible;
		bool m_bTwinkling;
		//HWND m_hWnd;
		//CPaintManagerUI* m_pManager;
		HICON m_hIcon;
		IDuiTimer* pIDuiTimer;
		NOTIFYICONDATA	m_trayData;
	};
}
#endif // DuiTrayIcon_h__

