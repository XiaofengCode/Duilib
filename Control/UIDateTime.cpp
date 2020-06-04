#include "..\duipub.h"
#include "UIDateTime.h"

namespace DuiLib
{
	//CDateTimeUI::m_nDTUpdateFlag
#define DT_NONE   0
#define DT_UPDATE 1
#define DT_DELETE 2
#define DT_KEEP   3

	class CDateTimeWnd : public CWindowWnd
	{
	public:
		CDateTimeWnd();

		void Init(CDateTimeUI* pOwner);
		RECT CalPos();

		LPCTSTR GetWindowClassName() const;
		LPCTSTR GetSuperClassName() const;
		void OnFinalMessage(HWND hWnd);

		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		//LRESULT OnEditChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	protected:
		CDateTimeUI* m_pOwner;
		HBRUSH m_hBkBrush;
		bool m_bInit;
	};

	CDateTimeWnd::CDateTimeWnd() : m_pOwner(NULL), m_hBkBrush(NULL), m_bInit(false)
	{
	}

	void CDateTimeWnd::Init(CDateTimeUI* pOwner)
	{
		m_pOwner = pOwner;
		RECT rcPos = CalPos();

		if (m_hWnd == NULL)
		{

			UINT uStyle = 0;//m_pOwner->GetDptStyle();
			if(m_pOwner->GetManager()->IsBackgroundTransparent())
			{
				uStyle = WS_POPUP | WS_VISIBLE;
				RECT rcWnd={0};
				::GetWindowRect(m_pOwner->GetManager()->GetPaintWindow(), &rcWnd);
				rcPos.left += rcWnd.left;
				rcPos.right += rcWnd.left;
				rcPos.top += rcWnd.top;
				rcPos.bottom += rcWnd.top;
			}
			else
			{
				uStyle = WS_CHILD;
			}

			if (m_pOwner->GetDptStyle() & DTS_TIMEFORMAT)
			{
				uStyle |= DTS_TIMEFORMAT;
			}
			else
			{
				uStyle |= DTS_SHORTDATEFORMAT;
			}
			//UINT uStyle = m_pOwner->GetDptStyle() | WS_CHILD;
			Create(m_pOwner->GetManager()->GetPaintWindow(), NULL, uStyle, 0, rcPos);
			SetWindowFont(m_hWnd, m_pOwner->GetManager()->GetFontInfo(m_pOwner->GetFont())->hFont, TRUE);
			if (m_pOwner->GetFormat())
			{
				DateTime_SetFormat(m_hWnd, m_pOwner->GetFormat());
			}
		}

		if (m_pOwner->GetText().IsEmpty())
			::GetLocalTime(&m_pOwner->m_sysTime);
		
		::SendMessage(m_hWnd, DTM_SETSYSTEMTIME, 0, (LPARAM)&m_pOwner->m_sysTime);
		::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
		::SetFocus(m_hWnd);

		m_bInit = true;    
	}

	RECT CDateTimeWnd::CalPos()
	{
		CDuiRect rcPos = m_pOwner->GetPos();
		return rcPos;
	}

	LPCTSTR CDateTimeWnd::GetWindowClassName() const
	{
		return _T("DateTimeWnd");
	}

	LPCTSTR CDateTimeWnd::GetSuperClassName() const
	{
		return DATETIMEPICK_CLASS;
	}

	void CDateTimeWnd::OnFinalMessage(HWND /*hWnd*/)
	{
		// Clear reference and die
		if( m_hBkBrush != NULL ) ::DeleteObject(m_hBkBrush);
		m_pOwner->m_pWindow = NULL;
		delete this;
	}

	LRESULT CDateTimeWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lRes = 0;
		BOOL bHandled = TRUE;
		if( uMsg == WM_KILLFOCUS )
		{
			lRes = OnKillFocus(uMsg, wParam, lParam, bHandled);
		}
// 		else if (uMsg == WM_KEYUP && (wParam == VK_DELETE || wParam == VK_BACK))
// 		{
// 			LRESULT lRes = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
// 			m_pOwner->m_nDTUpdateFlag = DT_DELETE;
// 			m_pOwner->UpdateText();
// 			PostMessage(WM_CLOSE);
// 			return lRes;
// 		}
		else if (uMsg == WM_KEYUP && wParam == VK_ESCAPE)
		{
			LRESULT lRes = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
			PostMessage(WM_CLOSE);
			return lRes;
		}
		else if( uMsg == OCM_NOTIFY)
		{
			CDuiString sDbg;
			sDbg.SmallFormat(_T("%X, %X\r\n"), wParam, lParam);
			OutputDebugString(sDbg);
			if(((LPNMHDR)lParam)->code == DTN_DATETIMECHANGE)
			{
				LPNMDATETIMECHANGE lpChangeParam = (LPNMDATETIMECHANGE)lParam;
				if (lpChangeParam->dwFlags == GDT_VALID)
				{
					//::SendMessage(m_hWnd, DTM_GETSYSTEMTIME, 0, (LPARAM)& m_pOwner->m_sysTime);
					m_pOwner->m_sysTime = lpChangeParam->st;
					m_pOwner->UpdateText();
				}
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
		else
		{
			bHandled = FALSE;
			CDuiString sDbg;
			sDbg.SmallFormat(_T("%X\r\n"), uMsg);
			OutputDebugString(sDbg);
		}
		if( !bHandled ) return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
		return lRes;
	}

	LRESULT CDateTimeWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LRESULT lRes = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
		::SendMessage(m_hWnd, DTM_GETSYSTEMTIME, 0, (LPARAM)&m_pOwner->m_sysTime);
		m_pOwner->UpdateText();
		PostMessage(WM_CLOSE);
		return lRes;
	}

	// LRESULT CDateTimeWnd::OnEditChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	// {
	// 	if( !m_bInit ) return 0;
	// 	if( m_pOwner == NULL ) return 0;
	// 	// Copy text back
	// 	int cchLen = ::GetWindowTextLength(m_hWnd) + 1;
	// 	LPTSTR pstr = static_cast<LPTSTR>(_alloca(cchLen * sizeof(TCHAR)));
	// 	ASSERT(pstr);
	// 	if( pstr == NULL ) return 0;
	// 	::GetWindowText(m_hWnd, pstr, cchLen);
	// 	m_pOwner->m_sText = pstr;
	// 	m_pOwner->GetManager()->SendNotify(m_pOwner, DUI_MSGTYPE_TEXTCHANGED);
	// 	return 0;
	// }
	//
	CDateTimeUI::CDateTimeUI()
	{
		m_uDptStyle = 0;
		::GetLocalTime(&m_sysTime);
		m_bReadOnly = false;
		m_pWindow = NULL;
		UpdateText();		// add by:daviyang35 初始化界面时显示时间
	}

	LPCTSTR CDateTimeUI::GetClass() const
	{
		return _T("DateTimeUI");
	}

	LPVOID CDateTimeUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_DATETIME) == 0 ) return static_cast<CDateTimeUI*>(this);
		return CLabelUI::GetInterface(pstrName);
	}

	SYSTEMTIME& CDateTimeUI::GetTime()
	{
		if (m_pWindow)
		{
			::SendMessage(m_pWindow->GetHWND(), DTM_GETSYSTEMTIME, 0, (LPARAM)&m_sysTime);
		}
		return m_sysTime;
	}

	void CDateTimeUI::SetTime(SYSTEMTIME* pst)
	{
		m_sysTime = *pst;
		if (m_sysTime.wMonth > 12) m_sysTime.wMonth = 12;
		if (m_sysTime.wDay > 31) m_sysTime.wDay = 1;
		if (m_sysTime.wHour > 24) m_sysTime.wHour = 0;
		if (m_sysTime.wMinute > 59) m_sysTime.wMinute = 0;
		if (m_sysTime.wSecond > 59) m_sysTime.wSecond = 0;
		if (m_sysTime.wMilliseconds > 1000) m_sysTime.wMilliseconds = 0;
		UpdateText();
	}

	void CDateTimeUI::SetReadOnly(bool bReadOnly)
	{
		m_bReadOnly = bReadOnly;
		Invalidate();
	}

	bool CDateTimeUI::IsReadOnly() const
	{
		return m_bReadOnly;
	}

	void CDateTimeUI::SetFormat(LPCTSTR lpszFormat)
	{
		m_strFormat = lpszFormat;
		UpdateText();
	}

	LPCTSTR CDateTimeUI::GetFormat()
	{
		if (m_strFormat.GetLength())
		{
			return m_strFormat;
		}
		return NULL;
	}

	void CDateTimeUI::UpdateText()
	{
		TCHAR szTime[64] = {0};
		TCHAR szDate[64] = {0};
		if (m_uDptStyle & DTS_TIMEFORMAT)
		{
			CDuiString strFormat = GetFormat();
			if (strFormat.GetLength() == 0)
			{
				GetTimeFormat(LOCALE_USER_DEFAULT, 0, &m_sysTime, NULL, szTime, sizeof(szTime)/sizeof(szTime[0]));
			}
			else
			{
				if (strFormat.Find(_T("y")) >= 0 || strFormat.Find(_T("M")) >= 0 || strFormat.Find(_T("d")) >= 0)
				{
					GetTimeFormat(LOCALE_USER_DEFAULT, 0, &m_sysTime, (LPCTSTR)strFormat, szDate, sizeof(szDate)/sizeof(szDate[0]));
					GetDateFormat(LOCALE_USER_DEFAULT, 0, &m_sysTime, szDate, szTime, sizeof(szTime)/sizeof(szTime[0]));
				}
				else
				{
					GetTimeFormat(LOCALE_USER_DEFAULT, 0, &m_sysTime, (LPCTSTR)strFormat, szTime, sizeof(szTime)/sizeof(szTime[0]));
				}
			}
		}
		else
		{
			CDuiString strFormat = GetFormat();
			if (strFormat.GetLength() == 0)
			{
				GetDateFormat(LOCALE_USER_DEFAULT, 0, &m_sysTime, NULL, szTime, sizeof(szTime)/sizeof(szTime[0]));
			}
			else
			{
				if (strFormat.Find(_T("H")) >= 0 || strFormat.Find(_T("h")) >= 0 ||
					strFormat.Find(_T("m")) >= 0 || strFormat.Find(_T("s")) >= 0)
				{
					GetDateFormat(LOCALE_USER_DEFAULT, 0, &m_sysTime, (LPCTSTR)strFormat, szDate, sizeof(szDate)/sizeof(szDate[0]));
					GetTimeFormat(LOCALE_USER_DEFAULT, 0, &m_sysTime, szDate, szTime, sizeof(szTime)/sizeof(szTime[0]));
				}
				else
				{
					GetDateFormat(LOCALE_USER_DEFAULT, 0, &m_sysTime, (LPCTSTR)strFormat, szTime, sizeof(szTime)/sizeof(szTime[0]));
				}
			}
		}			

		SetText(szTime);
	}

	void CDateTimeUI::DoEvent(TEventUI& event)
	{
		if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
			if( m_pParent != NULL ) m_pParent->DoEvent(event);
			else CLabelUI::DoEvent(event);
			return;
		}

		if( event.Type == UIEVENT_SETCURSOR && IsEnabled() && !IsFocused())
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
			if( m_pWindow ) return;
			m_pWindow = new CDateTimeWnd();
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
					m_pWindow = new CDateTimeWnd();
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

	void CDateTimeUI::SetAttribute( LPCTSTR pstrName, LPCTSTR pstrValue )
	{
		if (_tcsicmp(pstrName, _T("dtstyle")) == 0)
		{
			if (_tcsicmp(pstrValue, DTUI_TIMEFORMAT) == 0)
			{
				m_uDptStyle = DTS_TIMEFORMAT;
				UpdateText();
			}
			return;
		}
		if (_tcsicmp(pstrName, _T("format")) == 0)
		{
			SetFormat(pstrValue);
			return;
		}
		CLabelUI::SetAttribute(pstrName, pstrValue);
	}

	UINT CDateTimeUI::GetControlFlags() const
	{
		if( !IsEnabled() ) return CControlUI::GetControlFlags();

		return UIFLAG_SETCURSOR | UIFLAG_TABSTOP;
	}

}
