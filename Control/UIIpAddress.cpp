#include "..\duipub.h"
#include "UIDateTime.h"
#pragma comment( lib, "ws2_32.lib" )

DWORD GetDefaultIpAddress()   
{   
	//Windows跟Socket使用高低位相反
	DWORD dwIP = 0;//inet_addr("127.0.0.1");
	
	return dwIP;
}



namespace DuiLib
{
	//CDateTimeUI::m_nDTUpdateFlag
// #define IP_NONE   0
// #define IP_UPDATE 1
// #define IP_DELETE 2
// #define IP_KEEP   3

	class CIPAddressWnd : public CWindowWnd
	{
	public:
		CIPAddressWnd();

		void Init(CIPAddressUI* pOwner);
		RECT CalPos();

		LPCTSTR GetWindowClassName() const;
		LPCTSTR GetSuperClassName() const;
		void OnFinalMessage(HWND hWnd);

		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		LRESULT OnEditChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	protected:
		CIPAddressUI* m_pOwner;
		HBRUSH m_hBkBrush;
		bool m_bInit;
	};

	CIPAddressWnd::CIPAddressWnd() : m_pOwner(NULL), m_hBkBrush(NULL), m_bInit(false)
	{
	}

	void CIPAddressWnd::Init(CIPAddressUI* pOwner)
	{
		m_pOwner = pOwner;
//		m_pOwner->m_nIPUpdateFlag = IP_NONE;

		RECT rcPos = CalPos();
		if(m_pOwner->GetManager()->IsBackgroundTransparent())
		{
			RECT rcWnd={0};
			::GetWindowRect(m_pOwner->GetManager()->GetPaintWindow(), &rcWnd);
			rcPos.left += rcWnd.left;
			rcPos.right += rcWnd.left;
			rcPos.top += rcWnd.top;
			rcPos.bottom += rcWnd.top;
		}
		if (m_hWnd == NULL || !IsWindow(m_hWnd))
		{
			UINT uStyle = 0;
			if(m_pOwner->GetManager()->IsBackgroundTransparent())
			{
				uStyle = WS_POPUP | WS_VISIBLE;
			}
			else
			{
				uStyle = WS_CHILD;
			}

// 			INITCOMMONCONTROLSEX   CommCtrl;
// 			CommCtrl.dwSize=sizeof(CommCtrl);
// 			CommCtrl.dwICC=ICC_INTERNET_CLASSES;//指定Class
// 			if(InitCommonControlsEx(&CommCtrl))
// 			{
// 				RECT rcPos = CalPos();
// 				UINT uStyle = WS_CHILD | WS_TABSTOP | WS_GROUP;
// 			}
// 			CDuiString sDbg;
// 			sDbg.Format(_T("%d, %d, %d, %d"), rcPos.left, rcPos.top, rcPos.right, rcPos.bottom);
// 			MessageBox(0, sDbg, 0, 0);
			Create(m_pOwner->GetManager()->GetPaintWindow(), NULL, uStyle, 0, rcPos);
		}
		MoveWindow(m_hWnd, rcPos.left, rcPos.top, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top, false);
		LPCTSTR lpszFontIndex=m_pOwner->GetFont();
		HFONT hFont=m_pOwner->GetManager()->GetFont(lpszFontIndex);
		SetWindowFont(m_hWnd, hFont, TRUE);

		if (m_pOwner->GetText().IsEmpty())
			m_pOwner->m_dwIP = GetDefaultIpAddress();
		DWORD dwIP = m_pOwner->GetIP();
		::SendMessageA(m_hWnd, WM_SETTEXT, 0, (LPARAM)inet_ntoa(*(in_addr const *)&dwIP));
		//::SendMessage(m_hWnd, IPM_SETADDRESS, 0, m_pOwner->m_dwIP);
		::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
		//RedrawWindow(m_hWnd, NULL, NULL, 0);
		::SetFocus(m_hWnd);
		

		m_bInit = true;    
	}

	RECT CIPAddressWnd::CalPos()
	{
		CDuiRect rcPos = m_pOwner->GetPos();
		return rcPos;
	}

	LPCTSTR CIPAddressWnd::GetWindowClassName() const
	{
		return _T("IPAddressWnd");
	}

	LPCTSTR CIPAddressWnd::GetSuperClassName() const
	{
		return WC_IPADDRESS;
	}

	void CIPAddressWnd::OnFinalMessage(HWND /*hWnd*/)
	{
		// Clear reference and die
		if( m_hBkBrush != NULL ) ::DeleteObject(m_hBkBrush);
		m_pOwner->m_pWindow = NULL;
		delete this;
	}

	LRESULT CIPAddressWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lRes = 0;
		BOOL bHandled = TRUE;
		if( uMsg == WM_KILLFOCUS )
		{
			bHandled = TRUE;
			return 0;
			lRes = OnKillFocus(uMsg, wParam, lParam, bHandled);
		}
		else if (uMsg == WM_KEYUP && wParam == VK_ESCAPE)
		{
			LRESULT lRes = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
//			m_pOwner->m_nIPUpdateFlag = IP_KEEP;
			PostMessage(WM_CLOSE);
			return lRes;
		}
		else if( uMsg == OCM_COMMAND ) {
			if( GET_WM_COMMAND_CMD(wParam, lParam) == EN_KILLFOCUS )
			{
				lRes = OnKillFocus(uMsg, wParam, lParam, bHandled);
			}
			else if( GET_WM_COMMAND_CMD(wParam, lParam) == EN_CHANGE )
			{
				lRes = OnEditChanged(uMsg, wParam, lParam, bHandled);
			}
			else if( GET_WM_COMMAND_CMD(wParam, lParam) == EN_UPDATE )
			{
				RECT rcClient;
				::GetClientRect(m_hWnd, &rcClient);
				::InvalidateRect(m_hWnd, &rcClient, FALSE);
			}
		}
		//	else if( uMsg == WM_KEYDOWN && TCHAR(wParam) == VK_RETURN ) {
		// 		m_pOwner->GetManager()->SendNotify(m_pOwner, DUI_MSGTYPE_RETURN);
		//	}
		// 		else if( uMsg == OCM__BASE + WM_CTLCOLOREDIT  || uMsg == OCM__BASE + WM_CTLCOLORSTATIC ) {
		// 			if( m_pOwner->GetNativeEditBkColor() == 0xFFFFFFFF ) return NULL;
		// 			::SetBkMode((HDC)wParam, TRANSPARENT);
		// 			DWORD dwTextColor = m_pOwner->GetTextColor();
		// 			::SetTextColor((HDC)wParam, RGB(GetBValue(dwTextColor),GetGValue(dwTextColor),GetRValue(dwTextColor)));
		// 			if( m_hBkBrush == NULL ) {
		// 				DWORD clrColor = m_pOwner->GetNativeEditBkColor();
		// 				m_hBkBrush = ::CreateSolidBrush(RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
		// 			}
		// 			return (LRESULT)m_hBkBrush;
		// 		}
		else bHandled = FALSE;
		if( !bHandled ) return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
		return lRes;
	}

	LRESULT CIPAddressWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		//测试发现鼠标点到中间的点上会失焦，Windows的也是一个组合控件，
		//因此判断是不是当前控件的子控件如果是就忽略掉这个失焦消息
		HWND hWndFocus = GetFocus();
		while (hWndFocus)
		{
			if (GetFocus() == m_hWnd)
			{
				bHandled = TRUE;
				return 0;
			}
			hWndFocus = GetParent(hWndFocus);
		}

		LRESULT lRes = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
//		if (m_pOwner->m_nIPUpdateFlag == IP_NONE)
		{
			//Windows控件的IP地址和Socket使用的相反，所以要根据字符串转换一下
			DWORD dwIP;
			::SendMessage(m_hWnd, IPM_GETADDRESS, 0, (LPARAM)&dwIP);
			dwIP = htonl(dwIP);
			m_pOwner->SetIP(dwIP);
// 			char szIp[32] = {0};
// 			::SendMessageA(m_hWnd, WM_GETTEXT, sizeof(szIp), (LPARAM)szIp);
// 			m_pOwner->m_dwIP = inet_addr(szIp);
			//::SendMessage(m_hWnd, IPM_GETADDRESS, 0, (LPARAM)&m_pOwner->m_dwIP);
//			m_pOwner->m_nIPUpdateFlag = IP_UPDATE;
			m_pOwner->UpdateText();
		}
		::ShowWindow(m_hWnd, SW_HIDE);
		//PostMessage(WM_CLOSE);
		return lRes;
	}

	LRESULT CIPAddressWnd::OnEditChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		if( !m_bInit ) return 0;
		if( m_pOwner == NULL ) return 0;
		DWORD dwIP;
		::SendMessage(m_hWnd, IPM_GETADDRESS, 0, (LPARAM)&dwIP);
		dwIP = htonl(dwIP);
		m_pOwner->SetIP(dwIP);
		m_pOwner->GetManager()->SendNotify(m_pOwner, DUI_MSGTYPE_TEXTCHANGED);
		return 0;
	}

	//////////////////////////////////////////////////////////////////////////
	//
	CIPAddressUI::CIPAddressUI()
	{
		m_dwIP = GetDefaultIpAddress();
		m_bReadOnly = false;
		m_pWindow = NULL;
//		m_nIPUpdateFlag=IP_UPDATE;
		UpdateText();
//		m_nIPUpdateFlag = IP_NONE;
	}

	LPCTSTR CIPAddressUI::GetClass() const
	{
		return _T("IPAddressUI");
	}

	LPVOID CIPAddressUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_IPADDRESS) == 0 ) return static_cast<CIPAddressUI*>(this);
		return CLabelUI::GetInterface(pstrName);
	}

	DWORD CIPAddressUI::GetIP()
	{
		in_addr addr;
		CDuiString strText = GetText();
		int b1 = 0, b2 = 0, b3 = 0, b4 = 0;
		_stscanf((LPCTSTR)strText, _T("%d.%d.%d.%d"), &b1, &b2, &b3, &b4);
		addr.S_un.S_un_b.s_b1 = b1;
		addr.S_un.S_un_b.s_b2 = b2;
		addr.S_un.S_un_b.s_b3 = b3;
		addr.S_un.S_un_b.s_b4 = b4;
		m_dwIP = addr.S_un.S_addr;
		return m_dwIP;
	}

	void CIPAddressUI::SetIP(DWORD dwIP)
	{
		m_dwIP = dwIP;
		UpdateText();
	}

	void CIPAddressUI::SetReadOnly(bool bReadOnly)
	{
		m_bReadOnly = bReadOnly;
		Invalidate();
	}

	bool CIPAddressUI::IsReadOnly() const
	{
		return m_bReadOnly;
	}

	void CIPAddressUI::UpdateText()
	{
// 		if (m_nIPUpdateFlag == IP_DELETE)
// 			SetText(_T(""));
// 		else if (m_nIPUpdateFlag == IP_UPDATE)
		if (m_dwIP == 0)
		{
			SetText(_T("   .   .   .   "));
		}
		else
		{
			TCHAR szIP[MAX_PATH] = {0};
			in_addr addr;
			addr.S_un.S_addr = m_dwIP;
			_stprintf(szIP, _T("%3d.%3d.%3d.%3d"), addr.S_un.S_un_b.s_b1, addr.S_un.S_un_b.s_b2, addr.S_un.S_un_b.s_b3, addr.S_un.S_un_b.s_b4);
			SetText(szIP);
		}
	}

	void CIPAddressUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pParent != NULL ) m_pParent->DoEvent(event);
			else CLabelUI::DoEvent(event);
			return;
		}

		if( event.Type == UIEVENT_SETCURSOR && IsEnabled() )
		{
			::SetCursor(::LoadCursor(NULL, IDC_IBEAM));
			return;
		}
		if( event.Type == UIEVENT_WINDOWSIZE )
		{
			if( m_pWindow != NULL ) m_pManager->SetFocusNeeded(this);
		}
		if( event.Type == UIEVENT_SCROLLWHEEL )
		{
			if( m_pWindow != NULL ) return;
		}
		if( event.Type == UIEVENT_SETFOCUS && IsEnabled() ) 
		{
			if( m_pWindow ) 
			{
				m_pWindow->Init(this);
				m_pWindow->ShowWindow();
				return;
			}
			m_pWindow = new CIPAddressWnd();
			ASSERT(m_pWindow);
			m_pWindow->Init(this);
			m_pWindow->ShowWindow();
		}
		if( event.Type == UIEVENT_KILLFOCUS && IsEnabled() ) 
		{
			Invalidate();
		}
		if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK || event.Type == UIEVENT_RBUTTONDOWN) 
		{
			if( IsEnabled() ) {
				GetManager()->ReleaseCapture();
				if( IsFocused() && m_pWindow == NULL )
				{
					m_pWindow = new CIPAddressWnd();
					ASSERT(m_pWindow);
				}
				if( m_pWindow != NULL )
				{
					m_pWindow->Init(this);
					m_pWindow->ShowWindow();
				}
			}
			return;
		}
		if( event.Type == UIEVENT_MOUSEMOVE ) 
		{
			return;
		}
		if( event.Type == UIEVENT_BUTTONUP ) 
		{
			return;
		}
		if( event.Type == UIEVENT_CONTEXTMENU )
		{
			return;
		}
		if( event.Type == UIEVENT_MOUSEENTER )
		{
			return;
		}
		if( event.Type == UIEVENT_MOUSELEAVE )
		{
			return;
		}

		CLabelUI::DoEvent(event);
	}

	void CIPAddressUI::SetAttribute( LPCTSTR pstrName, LPCTSTR pstrValue )
	{
		CLabelUI::SetAttribute(pstrName, pstrValue);
	}

	DWORD CIPAddressUI::GetLocalIpAddress()   
	{   
		WORD wVersionRequested = MAKEWORD(2, 2);   
		WSADATA wsaData;   
		if (WSAStartup(wVersionRequested, &wsaData) != 0)   
			return 0;   
		char local[255] = {0};   
		gethostname(local, sizeof(local));   
		hostent* ph = gethostbyname(local);   
		if (ph == NULL)   
			return 0;   
		in_addr addr;   
		memcpy(&addr, ph->h_addr_list[0], sizeof(in_addr));   
		DWORD dwIP = MAKEIPADDRESS(addr.S_un.S_un_b.s_b1, addr.S_un.S_un_b.s_b2, addr.S_un.S_un_b.s_b3, addr.S_un.S_un_b.s_b4);
		WSACleanup();
		return dwIP;
	}

	UINT CIPAddressUI::GetControlFlags() const
	{
		if( !IsEnabled() ) return CControlUI::GetControlFlags();

		return UIFLAG_SETCURSOR | UIFLAG_TABSTOP;
	}

	void CIPAddressUI::SetPos(RECT rc)
	{
		CControlUI::SetPos(rc);
		if( m_pWindow != NULL ) {
			RECT rcPos = m_pWindow->CalPos();
			::SetWindowPos(m_pWindow->GetHWND(), NULL, rcPos.left, rcPos.top, rcPos.right - rcPos.left, 
				rcPos.bottom - rcPos.top, SWP_NOZORDER | SWP_NOACTIVATE);        
		}
	}

}
