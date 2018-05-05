#include "StdAfx.h"
#include "DuiTrayIcon.h"

namespace DuiLib
{

	CDuiTrayIconUI::CDuiTrayIconUI(void)
	{
		memset(&m_trayData, 0, sizeof(m_trayData));
		m_bEnabled		= false;
		m_bVisible		= false;
		m_bTwinkling	= false;
		//m_hWnd			= NULL;
		m_pManager		= NULL;
		pIDuiTimer		= NULL;
	}


	CDuiTrayIconUI::~CDuiTrayIconUI(void)
	{
		DeleteTrayIcon();
	}

	void CDuiTrayIconUI::CreateTrayIcon( CPaintManagerUI* pManager,UINT uIconIDResource,LPCTSTR lpszToolTipText /*= NULL*/,UINT dwMessage)
	{
		HWND hWnd = pManager->GetPaintWindow();
		if(!hWnd || !uIconIDResource )
			return;

		if (!dwMessage)
		{
			dwMessage = WM_DUI_TRAYICON_MESSAGE;
		}
		m_hIcon = LoadIcon(m_pManager->GetInstance(),MAKEINTRESOURCE(uIconIDResource));

		m_trayData.cbSize = sizeof(NOTIFYICONDATA);
		m_trayData.hWnd	 = hWnd;
		m_trayData.uID	 = uIconIDResource;
		m_trayData.hIcon = m_hIcon;
		m_trayData.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
		m_trayData.uCallbackMessage = dwMessage;
		if(lpszToolTipText)
			_tcscpy(m_trayData.szTip,lpszToolTipText);

		Shell_NotifyIcon(NIM_ADD,&m_trayData);
		m_bEnabled = true;
		m_pManager = pManager;
	}

	void CDuiTrayIconUI::CreateTrayIcon( CPaintManagerUI* pManager,LPCTSTR lpszIconPath,LPCTSTR lpszToolTipText /*= NULL*/,UINT dwMessage )
	{
		HWND hWnd = pManager->GetPaintWindow();
		if(!hWnd || !lpszIconPath )
			return;

		if (!dwMessage)
		{
			dwMessage = WM_DUI_TRAYICON_MESSAGE;
		}
		m_hIcon = (HICON)LoadImage(
			NULL,    //handle of the instance that contains //the image
			lpszIconPath,//name or identifier of image
			IMAGE_ICON,//type of image-can also be IMAGE_CURSOR or MAGE_ICON
			0,0,//desired width and height
			LR_LOADFROMFILE);//load flags

		m_trayData.cbSize = sizeof(NOTIFYICONDATA);
		m_trayData.hWnd	 = hWnd;
		m_trayData.uID	 = 0;
		m_trayData.hIcon = m_hIcon;
		m_trayData.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
		m_trayData.uCallbackMessage = dwMessage;
		if(lpszToolTipText)
			_tcscpy(m_trayData.szTip,lpszToolTipText);

		Shell_NotifyIcon(NIM_ADD, &m_trayData);
		m_bEnabled = true;
		m_pManager = pManager;
	}

	void CDuiTrayIconUI::DeleteTrayIcon()
	{
		if(pIDuiTimer)
			StopTwinkling();

		Shell_NotifyIcon(NIM_DELETE,&m_trayData);
		m_bEnabled		= false;
		m_bVisible		= false;
		m_bTwinkling	= false;
		//m_hWnd			= NULL;
		m_pManager		= NULL;
	}

	bool CDuiTrayIconUI::SetTooltipText( LPCTSTR _ToolTipText )
	{
		if(_ToolTipText)
			_tcscpy(m_trayData.szTip,_ToolTipText);

		if (!m_bEnabled) return FALSE;
		m_trayData.uFlags = NIF_TIP;
		return Shell_NotifyIcon(NIM_MODIFY, &m_trayData) == TRUE;
	}

	bool CDuiTrayIconUI::SetTooltipText( UINT _IDResource )
	{
		TCHAR mbuf[64];
		LoadString(m_pManager->GetInstance(), _IDResource, mbuf, 64);

		return SetTooltipText(mbuf);
	}

	CDuiString CDuiTrayIconUI::GetTooltipText() const
	{
		return m_trayData.szTip;
	}

	bool CDuiTrayIconUI::SetIcon( HICON _Hicon )
	{
		if(_Hicon)
			m_hIcon = _Hicon;

		m_trayData.uFlags = NIF_ICON;
		m_trayData.hIcon = _Hicon;
		
		if (!m_bEnabled) return FALSE;
		return Shell_NotifyIcon(NIM_MODIFY, &m_trayData) == TRUE;

		return false;
	}

	bool CDuiTrayIconUI::SetIcon( LPCTSTR _IconFile )
	{
		HICON hIcon = LoadIcon(m_pManager->GetInstance(),_IconFile);
		return SetIcon(hIcon);
	}

	bool CDuiTrayIconUI::SetIcon( UINT _IDResource )
	{
		HICON hIcon = LoadIcon(m_pManager->GetInstance(),MAKEINTRESOURCE(_IDResource));
		return SetIcon(hIcon);
	}

	HICON CDuiTrayIconUI::GetIcon() const
	{
		HICON hIcon = NULL;
		hIcon = m_trayData.hIcon;
		return hIcon;
	}

	void CDuiTrayIconUI::SetHideIcon()
	{
		if (IsVisible()) {
			SetIcon((HICON)NULL);
			m_bVisible = TRUE;
		}
	}

	void CDuiTrayIconUI::SetShowIcon()
	{
		if (!IsVisible()) {
			SetIcon(m_hIcon);
			m_bVisible = FALSE;
		}
	}

	void CDuiTrayIconUI::RemoveIcon()
	{
		m_trayData.uFlags = 0;
		Shell_NotifyIcon(NIM_DELETE, &m_trayData);
		m_bEnabled = FALSE;
	}

	bool CDuiTrayIconUI::StartTwinkling()
	{
		if(m_bTwinkling || !m_bEnabled || pIDuiTimer)
			return false;

		pIDuiTimer = MakeDuiTimer(this,&CDuiTrayIconUI::OnTimer,400);
		pIDuiTimer->SetDuiTimer();
		m_bTwinkling = true;
		return true;
	}

	void CDuiTrayIconUI::StopTwinkling()
	{
		if(pIDuiTimer){
			pIDuiTimer->KillDuiTimer();
			delete pIDuiTimer;
			pIDuiTimer = NULL;
		}
		m_bTwinkling = false;
		SetShowIcon();
	}

	void CDuiTrayIconUI::OnTimer( IDuiTimer* pTimer )
	{
		IsVisible()?SetHideIcon():SetShowIcon();
	}

	bool CDuiTrayIconUI::ShowMessage(LPCTSTR lpMsg, LPCTSTR lpTitle, UINT uTimeout)
	{
		if (!m_bEnabled) return false;
		m_trayData.uFlags = NIF_INFO;
		m_trayData.uTimeout = uTimeout;
		m_trayData.dwInfoFlags = NIIF_INFO;
		lstrcpy( m_trayData.szInfo, lpMsg );
		lstrcpy( m_trayData.szInfoTitle, lpTitle ? lpTitle : TEXT( "" ) );
		return Shell_NotifyIcon( NIM_MODIFY, &m_trayData ) ? true : false;
	}
}
