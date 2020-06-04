#include "..\duipub.h"

#include "UIMenu.h"

namespace DuiLib {

//

CMenuUI::CMenuUI()
{
	if (GetHeader() != NULL)
		GetHeader()->SetVisible(false);
}

CMenuUI::~CMenuUI()
{}

LPCTSTR CMenuUI::GetClass() const
{
    return _T("MenuUI");
}

LPVOID CMenuUI::GetInterface(LPCTSTR pstrName)
{
    if( _tcsicmp(pstrName, _T("Menu")) == 0 ) return static_cast<CMenuUI*>(this);
    return CListUI::GetInterface(pstrName);
}

void CMenuUI::DoEvent(TEventUI& event)
{
	return __super::DoEvent(event);
}

bool CMenuUI::Add(CControlUI* pControl)
{
	CMenuElementUI* pMenuItem = static_cast<CMenuElementUI*>(pControl->GetInterface(_T("MenuElement")));
	if (pMenuItem == NULL)
		return false;

	for (int i = 0; i < pMenuItem->GetCount(); ++i)
	{
		if (pMenuItem->GetItemAt(i)->GetInterface(_T("MenuElement")) != NULL)
		{
			(static_cast<CMenuElementUI*>(pMenuItem->GetItemAt(i)->GetInterface(_T("MenuElement"))))->SetInternVisible(false);
		}
	}
	return CListUI::Add(pControl);
}

bool CMenuUI::AddAt(CControlUI* pControl, int iIndex)
{
	CMenuElementUI* pMenuItem = static_cast<CMenuElementUI*>(pControl->GetInterface(_T("MenuElement")));
	if (pMenuItem == NULL)
		return false;

	for (int i = 0; i < pMenuItem->GetCount(); ++i)
	{
		if (pMenuItem->GetItemAt(i)->GetInterface(_T("MenuElement")) != NULL)
		{
			(static_cast<CMenuElementUI*>(pMenuItem->GetItemAt(i)->GetInterface(_T("MenuElement"))))->SetInternVisible(false);
		}
	}
	return CListUI::AddAt(pControl, iIndex);
}

int CMenuUI::GetItemIndex(CControlUI* pControl) const
{
	CMenuElementUI* pMenuItem = static_cast<CMenuElementUI*>(pControl->GetInterface(_T("MenuElement")));
	if (pMenuItem == NULL)
		return -1;

	return __super::GetItemIndex(pControl);
}

bool CMenuUI::SetItemIndex(CControlUI* pControl, int iIndex)
{
	CMenuElementUI* pMenuItem = static_cast<CMenuElementUI*>(pControl->GetInterface(_T("MenuElement")));
	if (pMenuItem == NULL)
		return false;

	return __super::SetItemIndex(pControl, iIndex);
}

bool CMenuUI::Remove(CControlUI* pControl)
{
	CMenuElementUI* pMenuItem = static_cast<CMenuElementUI*>(pControl->GetInterface(_T("MenuElement")));
	if (pMenuItem == NULL)
		return false;

	return __super::Remove(pControl);
}

SIZE CMenuUI::EstimateSize(SIZE szAvailable)
{
	int cxFixed = 0;
    int cyFixed = 0;
    for( int it = 0; it < GetCount(); it++ ) {
        CControlUI* pControl = static_cast<CControlUI*>(GetItemAt(it));
        if( !pControl->IsVisible() ) continue;
        SIZE sz = pControl->EstimateSize(szAvailable);
        cyFixed += sz.cy;
		if( cxFixed < sz.cx )
			cxFixed = sz.cx;
    }
    return CDuiSize(cxFixed, cyFixed);
}

void CMenuUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	__super::SetAttribute(pstrName, pstrValue);
}

//

CMenuWnd::CMenuWnd():
m_pOwner(NULL),
m_pLayout(),
m_xml(_T(""))
{
	m_dwAlignment = eMenuAlignment_Left | eMenuAlignment_Top;
	m_pAttrbuteCallback = NULL;
}

CMenuWnd::~CMenuWnd()
{

}

BOOL CMenuWnd::Receive(ContextMenuParam param)
{
	switch (param.wParam)
	{
	case 1:
		Close();
		break;
	case 2:
		{
			HWND hParent = GetParent(m_hWnd);
			while (hParent != NULL)
			{
				if (hParent == param.hWnd)
				{
					Close();
					break;
				}
				hParent = GetParent(hParent);
			}
		}
		break;
	default:
		break;
	}

	return TRUE;
}

void CMenuWnd::Init(CMenuElementUI* pOwner, STRINGorID xml, POINT point,
					CPaintManagerUI* pMainPaintManager, std::map<CDuiString,bool>* pMenuCheckInfo/* = NULL*/,
					DWORD dwAlignment/* = eMenuAlignment_Left | eMenuAlignment_Top*/,
					IDialogBuilderAttrbuteCallback * pAttrbuteCallback /*= NULL*/)
{

	m_BasedPoint = point;
    m_pOwner = pOwner;
    m_pLayout = NULL;
	m_xml = xml;
	m_dwAlignment = dwAlignment;
	m_pAttrbuteCallback = pAttrbuteCallback;

	// 如果是一级菜单的创建
	if (pOwner == NULL)
	{
		ASSERT(pMainPaintManager != NULL);
		GetGlobalContextMenuObserver().SetManger(pMainPaintManager);
		if (pMenuCheckInfo != NULL)
			GetGlobalContextMenuObserver().SetMenuCheckInfo(pMenuCheckInfo);
	}

	GetGlobalContextMenuObserver().AddReceiver(this);

	Create((m_pOwner == NULL) ? pMainPaintManager->GetPaintWindow() : m_pOwner->GetManager()->GetPaintWindow(), NULL, WS_POPUP , WS_EX_TOOLWINDOW | WS_EX_TOPMOST, CDuiRect());

	// HACK: Don't deselect the parent's caption
    HWND hWndParent = m_hWnd;
    while( ::GetParent(hWndParent) != NULL ) hWndParent = ::GetParent(hWndParent);

    ::ShowWindow(m_hWnd, SW_SHOW);
    ::SendMessage(hWndParent, WM_NCACTIVATE, TRUE, 0L);
}

LPCTSTR CMenuWnd::GetWindowClassName() const
{
    return _T("DuiMenuWnd");
}


void CMenuWnd::Notify(TNotifyUI& msg)
{
	if( CMenuWnd::GetGlobalContextMenuObserver().GetManager() != NULL) 
	{
		CMenuWnd::GetGlobalContextMenuObserver().GetManager()->SendNotify(msg, msg.bAsync);
	}

}

CControlUI* CMenuWnd::CreateControl( LPCTSTR pstrClassName )
{
		if (_tcsicmp(pstrClassName, _T("Menu")) == 0)
		{
			return new CMenuUI();
		}
		else if (_tcsicmp(pstrClassName, _T("MenuElement")) == 0)
		{
			return new CMenuElementUI();
		}
		return NULL;
}


void CMenuWnd::OnFinalMessage(HWND hWnd)
{
	if (CMenuWnd::GetGlobalContextMenuObserver().GetManager() != NULL)
	{
		::PostMessage(CMenuWnd::GetGlobalContextMenuObserver().GetManager()->GetPaintWindow(), WM_MENUDESTORY, (WPARAM)hWnd, 0);
	}

	RemoveObserver();
	if( m_pOwner != NULL )
	{
		for( int i = 0; i < m_pOwner->GetCount(); i++ )
		{
			CMenuElementUI* pItem = static_cast<CMenuElementUI*>(m_pOwner->GetItemAt(i)->GetInterface(_T("MenuElement")));
			if( pItem )
			{
				pItem->SetOwner(m_pOwner->GetParent());
				pItem->SetVisible(false);
				pItem->SetInternVisible(false);
			}
		}
		m_pOwner->m_pWindow = NULL;
		m_pOwner->m_uButtonState &= ~ UISTATE_PUSHED;
		m_pOwner->Invalidate();
	}
    delete this;
}

LRESULT CMenuWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if( m_pOwner != NULL) {
		LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
		styleValue &= ~WS_CAPTION;
		::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
		RECT rcClient;
		::GetClientRect(*this, &rcClient);
		::SetWindowPos(*this, NULL, rcClient.left, rcClient.top, rcClient.right - rcClient.left, \
			rcClient.bottom - rcClient.top, SWP_FRAMECHANGED);

		m_pm.Init(m_hWnd);
		// The trick is to add the items to the new container. Their owner gets
		// reassigned by this operation - which is why it is important to reassign
		// the items back to the righfull owner/manager when the window closes.
		m_pLayout = new CMenuUI();
		m_pm.UseParentResource(m_pOwner->GetManager());
		m_pLayout->SetManager(&m_pm, NULL, true);
		LPCTSTR pDefaultAttributes = m_pOwner->GetManager()->GetDefaultAttributeList(_T("Menu"));
		if( pDefaultAttributes ) {
			m_pLayout->ApplyAttributeList(pDefaultAttributes);
		}
		m_pLayout->SetAutoDestroy(false);

		for( int i = 0; i < m_pOwner->GetCount(); i++ )
		{
			CControlUI* pCtrl = m_pOwner->GetItemAt(i);
			if(pCtrl->GetInterface(_T("MenuElement")) != NULL )
			{
				(static_cast<CMenuElementUI*>(pCtrl))->SetOwner(m_pLayout);
				if (m_pLayout->Add(pCtrl))
					continue;
			}
		}

		CShadowUI *pShadow = m_pOwner->GetManager()->GetShadow();
		pShadow->CopyShadow(m_pm.GetShadow());

		pShadow->ShowShadow(false);

		m_pm.AttachDialog(m_pLayout);
		m_pm.AddNotifier(this);
		// Position the popup window in absolute space
		RECT rcOwner = m_pOwner->GetPos();
		::MapWindowRect(m_pOwner->GetManager()->GetPaintWindow(), HWND_DESKTOP, &rcOwner);
		RECT rc = rcOwner;

		int cxFixed = 0;
		int cyFixed = 0;

#if defined(WIN32) && !defined(UNDER_CE)
		MONITORINFO oMonitor = {}; 
		oMonitor.cbSize = sizeof(oMonitor);
		::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
		CDuiRect rcWork = oMonitor.rcWork;
#else
		CDuiRect rcWork;
		GetWindowRect(m_pOwner->GetManager()->GetPaintWindow(), &rcWork);
#endif
		SIZE szAvailable = { rcWork.right - rcWork.left, rcWork.bottom - rcWork.top };

		for( int it = 0; it < m_pOwner->GetCount(); it++ ) {
			if(m_pOwner->GetItemAt(it)->GetInterface(_T("MenuElement")) != NULL ){
				CControlUI* pControl = static_cast<CControlUI*>(m_pOwner->GetItemAt(it));
				SIZE sz = pControl->EstimateSize(szAvailable);
				cyFixed += sz.cy;

				if( cxFixed < sz.cx )
					cxFixed = sz.cx;
			}
		}

		RECT rcWindow;
		GetWindowRect(m_pOwner->GetManager()->GetPaintWindow(), &rcWindow);

		rc.top = rcOwner.top;
		rc.bottom = rc.top + cyFixed;
		rc.left = rcWindow.right;
		rc.right = rc.left + cxFixed;
		rc.right += 2;

		bool bReachBottom = false;
		bool bReachRight = false;
		LONG chRightAlgin = 0;
		LONG chBottomAlgin = 0;

		RECT rcPreWindow = {0};
		ObserverImpl::Iterator iterator(CMenuWnd::GetGlobalContextMenuObserver());
		ReceiverImplBase* pReceiver = iterator.next();
		while( pReceiver != NULL ) {
			CMenuWnd* pContextMenu = dynamic_cast<CMenuWnd*>(pReceiver);
			if( pContextMenu != NULL ) {
				GetWindowRect(pContextMenu->GetHWND(), &rcPreWindow);

				bReachRight = rcPreWindow.left >= rcWindow.right;
				bReachBottom = rcPreWindow.top >= rcWindow.bottom;
				if( pContextMenu->GetHWND() == m_pOwner->GetManager()->GetPaintWindow() 
					||  bReachBottom || bReachRight )
					break;
			}
			pReceiver = iterator.next();
		}

		if (bReachBottom)
		{
			rc.bottom = rcOwner.bottom;//rcWindow.top;
			rc.top = rc.bottom - cyFixed;
		}

		if (bReachRight)
		{
			rc.right = rcWindow.left;
			rc.left = rc.right - cxFixed;
		}

		if( rc.bottom > rcWork.bottom )
		{
			rc.bottom = rcOwner.bottom;//rc.top;
			rc.top = rc.bottom - cyFixed;
		}

		if (rc.right > rcWork.right)
		{
			rc.right = rcWindow.left;
			rc.left = rc.right - cxFixed;
		}

		if( rc.top < rcWork.top )
		{
			rc.top = rcOwner.top;
			rc.bottom = rc.top + cyFixed;
		}

		if (rc.left < rcWork.left)
		{
			rc.left = rcWindow.right;
			rc.right = rc.left + cxFixed;
		}

		MoveWindow(m_hWnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top + m_pLayout->GetInset().top + m_pLayout->GetInset().bottom, FALSE);

	}
	else {
		m_pm.Init(m_hWnd);

		CDialogBuilder builder;
		builder.SetAttrbuteCallBack(m_pAttrbuteCallback);

		CControlUI* pRoot = builder.Create(m_xml ,UINT(0), this, &m_pm);
		m_pm.GetShadow()->ShowShadow(false);
		m_pm.AttachDialog(pRoot);
		m_pm.AddNotifier(this);
#if defined(WIN32) && !defined(UNDER_CE)
		MONITORINFO oMonitor = {}; 
		oMonitor.cbSize = sizeof(oMonitor);
		::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
		CDuiRect rcWork = oMonitor.rcWork;
#else
		CDuiRect rcWork;
		GetWindowRect(m_pOwner->GetManager()->GetPaintWindow(), &rcWork);
#endif
		SIZE szAvailable = { rcWork.right - rcWork.left, rcWork.bottom - rcWork.top };
		szAvailable = pRoot->EstimateSize(szAvailable);
		m_pm.SetInitSize(szAvailable.cx, szAvailable.cy);

		//必须是Menu标签作为xml的根节点
		CMenuUI *pMenuRoot = static_cast<CMenuUI*>(pRoot);
		ASSERT(pMenuRoot);

		SIZE szInit = m_pm.GetInitSize();
		CDuiRect rc;
		CDuiPoint point = m_BasedPoint;
		rc.left = point.x;
		rc.top = point.y;
		rc.right = rc.left + szInit.cx;
		rc.bottom = rc.top + szInit.cy;

		int nWidth = rc.GetWidth();
		int nHeight = rc.GetHeight();

		if (m_dwAlignment & eMenuAlignment_Right)
		{
			rc.right = point.x;
			rc.left = rc.right - nWidth;
		}

		if (m_dwAlignment & eMenuAlignment_Bottom)
		{
			rc.bottom = point.y;
			rc.top = rc.bottom - nHeight;
		}


		nWidth = rc.GetWidth();
		nHeight = rc.GetHeight();

		if (rc.bottom > rcWork.bottom)
		{
			rc.bottom = point.y;
			rc.top = rc.bottom - nHeight;
		}

		if (rc.right > rcWork.right)
		{
			rc.right = point.x;
			rc.left = rc.right - nWidth;
		}


		SetForegroundWindow(m_hWnd);
		MoveWindow(m_hWnd, rc.left, rc.top, rc.GetWidth(), rc.GetHeight(), FALSE);
		SetWindowPos(m_hWnd, HWND_TOPMOST, rc.left, rc.top,
			rc.GetWidth(), rc.GetHeight() + pMenuRoot->GetInset().bottom + pMenuRoot->GetInset().top,
			SWP_SHOWWINDOW);
	}

	m_pm.GetShadow()->ShowShadow(true);
	m_pm.GetShadow()->Create(&m_pm);
	return 0;
}


CMenuUI* CMenuWnd::GetMenuUI()
{
	return static_cast<CMenuUI*>(m_pm.GetRoot());
}

void CMenuWnd::ResizeMenu()
{
	CControlUI* pRoot = m_pm.GetRoot();

#if defined(WIN32) && !defined(UNDER_CE)
	MONITORINFO oMonitor = {};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
	CDuiRect rcWork = oMonitor.rcWork;
#else
	CDuiRect rcWork;
	GetWindowRect(m_pOwner->GetManager()->GetPaintWindow(), &rcWork);
#endif
	SIZE szAvailable = { rcWork.right - rcWork.left, rcWork.bottom - rcWork.top };
	szAvailable = pRoot->EstimateSize(szAvailable);
	m_pm.SetInitSize(szAvailable.cx, szAvailable.cy);

	//必须是Menu标签作为xml的根节点
	CMenuUI *pMenuRoot = static_cast<CMenuUI*>(pRoot);
	ASSERT(pMenuRoot);

	SIZE szInit = m_pm.GetInitSize();
	CDuiRect rc;
	CDuiPoint point = m_BasedPoint;
	rc.left = point.x;
	rc.top = point.y;
	rc.right = rc.left + szInit.cx;
	rc.bottom = rc.top + szInit.cy;

	int nWidth = rc.GetWidth();
	int nHeight = rc.GetHeight();

	if (m_dwAlignment & eMenuAlignment_Right)
	{
		rc.right = point.x;
		rc.left = rc.right - nWidth;
	}

	if (m_dwAlignment & eMenuAlignment_Bottom)
	{
		rc.bottom = point.y;
		rc.top = rc.bottom - nHeight;
	}


	nWidth = rc.GetWidth();
	nHeight = rc.GetHeight();

	if (rc.bottom > rcWork.bottom)
	{
		rc.bottom = point.y;
		rc.top = rc.bottom - nHeight;
	}

	if (rc.right > rcWork.right)
	{
		rc.right = point.x;
		rc.left = rc.right - nWidth;
	}

	SetForegroundWindow(m_hWnd);
	MoveWindow(m_hWnd, rc.left, rc.top, rc.GetWidth(), rc.GetHeight(), FALSE);
	SetWindowPos(m_hWnd, HWND_TOPMOST, rc.left, rc.top,
		rc.GetWidth(), rc.GetHeight() + pMenuRoot->GetInset().bottom + pMenuRoot->GetInset().top,
		SWP_SHOWWINDOW);
}

void CMenuWnd::ResizeSubMenu()
{
	if (!m_pOwner)
	{
		return;
	}

	// Position the popup window in absolute space
	RECT rcOwner = m_pOwner->GetPos();
	RECT rc = rcOwner;

	int cxFixed = 0;
	int cyFixed = 0;

#if defined(WIN32) && !defined(UNDER_CE)
	MONITORINFO oMonitor = {};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
	CDuiRect rcWork = oMonitor.rcWork;
#else
	CDuiRect rcWork;
	GetWindowRect(m_pOwner->GetManager()->GetPaintWindow(), &rcWork);
#endif
	SIZE szAvailable = { rcWork.right - rcWork.left, rcWork.bottom - rcWork.top };

	for (int it = 0; it < m_pOwner->GetCount(); it++) {
		if (m_pOwner->GetItemAt(it)->GetInterface(_T("MenuElement")) != NULL){
			CControlUI* pControl = static_cast<CControlUI*>(m_pOwner->GetItemAt(it));
			SIZE sz = pControl->EstimateSize(szAvailable);
			cyFixed += sz.cy;

			if (cxFixed < sz.cx)
				cxFixed = sz.cx;
		}
	}

	RECT rcWindow;
	GetWindowRect(m_pOwner->GetManager()->GetPaintWindow(), &rcWindow);

	rc.top = rcOwner.top;
	rc.bottom = rc.top + cyFixed;
	::MapWindowRect(m_pOwner->GetManager()->GetPaintWindow(), HWND_DESKTOP, &rc);
	rc.left = rcWindow.right;
	rc.right = rc.left + cxFixed;
	rc.right += 2;

	bool bReachBottom = false;
	bool bReachRight = false;
	LONG chRightAlgin = 0;
	LONG chBottomAlgin = 0;

	RECT rcPreWindow = { 0 };
	ObserverImpl::Iterator iterator(CMenuWnd::GetGlobalContextMenuObserver());
	ReceiverImplBase* pReceiver = iterator.next();
	while (pReceiver != NULL) {
		CMenuWnd* pContextMenu = dynamic_cast<CMenuWnd*>(pReceiver);
		if (pContextMenu != NULL) {
			GetWindowRect(pContextMenu->GetHWND(), &rcPreWindow);

			bReachRight = rcPreWindow.left >= rcWindow.right;
			bReachBottom = rcPreWindow.top >= rcWindow.bottom;
			if (pContextMenu->GetHWND() == m_pOwner->GetManager()->GetPaintWindow()
				|| bReachBottom || bReachRight)
				break;
		}
		pReceiver = iterator.next();
	}

	if (bReachBottom)
	{
		rc.bottom = rcWindow.top;
		rc.top = rc.bottom - cyFixed;
	}

	if (bReachRight)
	{
		rc.right = rcWindow.left;
		rc.left = rc.right - cxFixed;
	}

	if (rc.bottom > rcWork.bottom)
	{
		rc.bottom = rc.top;
		rc.top = rc.bottom - cyFixed;
	}

	if (rc.right > rcWork.right)
	{
		rc.right = rcWindow.left;
		rc.left = rc.right - cxFixed;
	}

	if (rc.top < rcWork.top)
	{
		rc.top = rcOwner.top;
		rc.bottom = rc.top + cyFixed;
	}

	if (rc.left < rcWork.left)
	{
		rc.left = rcWindow.right;
		rc.right = rc.left + cxFixed;
	}

	MoveWindow(m_hWnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top + m_pLayout->GetInset().top + m_pLayout->GetInset().bottom, FALSE);
}

LRESULT CMenuWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HWND hFocusWnd = (HWND)wParam;

	BOOL bInMenuWindowList = FALSE;
	ContextMenuParam param;
	param.hWnd = GetHWND();

	ObserverImpl::Iterator iterator(CMenuWnd::GetGlobalContextMenuObserver());
	ReceiverImplBase* pReceiver = iterator.next();
	while( pReceiver != NULL ) {
		CMenuWnd* pContextMenu = dynamic_cast<CMenuWnd*>(pReceiver);
		if( pContextMenu != NULL && pContextMenu->GetHWND() ==  hFocusWnd ) {
			bInMenuWindowList = TRUE;
			break;
		}
		pReceiver = iterator.next();
	}

	if( !bInMenuWindowList ) {
		param.wParam = 1;
		CMenuWnd::GetGlobalContextMenuObserver().RBroadcast(param);

		return 0;
	}
	return 0;
}
LRESULT CMenuWnd::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SIZE szRoundCorner = m_pm.GetRoundCorner();
	if( !::IsIconic(*this) ) {
		CDuiRect rcWnd;
		::GetWindowRect(*this, &rcWnd);
		rcWnd.Offset(-rcWnd.left, -rcWnd.top);
		rcWnd.right++; rcWnd.bottom++;
		HRGN hRgn = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, szRoundCorner.cx, szRoundCorner.cy);
		::SetWindowRgn(*this, hRgn, TRUE);
		::DeleteObject(hRgn);
	}
	bHandled = FALSE;
	return 0;
}

LRESULT CMenuWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;
	switch( uMsg )
	{
	case WM_CREATE:       
		lRes = OnCreate(uMsg, wParam, lParam, bHandled); 
		break;
	case WM_KILLFOCUS:       
		lRes = OnKillFocus(uMsg, wParam, lParam, bHandled); 
		break;
	case WM_KEYDOWN:
		if( wParam == VK_ESCAPE || wParam == VK_LEFT)
			Close();
		break;
	case WM_SIZE:
		lRes = OnSize(uMsg, wParam, lParam, bHandled);
		break;
	case WM_CLOSE:
		if( m_pOwner != NULL )
		{
			m_pOwner->SetManager(m_pOwner->GetManager(), m_pOwner->GetParent(), false);
			m_pOwner->SetPos(m_pOwner->GetPos());
			m_pOwner->SetFocus();
		}
		break;
	case WM_RBUTTONDOWN:
	case WM_CONTEXTMENU:
	case WM_RBUTTONUP:
	case WM_RBUTTONDBLCLK:
		return 0L;
		break;
	default:
		bHandled = FALSE;
		break;
	}

    if( m_pm.MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
    return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

//

CMenuElementUI::CMenuElementUI():
m_pWindow(NULL),
m_bDrawLine(false),
m_dwLineColor(DEFAULT_LINE_COLOR),
m_bCheckItem(false),
m_bShowExplandIcon(false),
m_uTextStyle(DT_LEFT|DT_VCENTER)
{
	m_attrs.SetAttribute(DUI_ATTR_WIDTH, ITEM_DEFAULT_WIDTH, true);
	m_attrs.SetAttribute(DUI_ATTR_HEIGHT, ITEM_DEFAULT_HEIGHT, true);
	m_szIconSize.cy = ITEM_DEFAULT_ICON_SIZE;
	m_szIconSize.cx = ITEM_DEFAULT_ICON_SIZE;

	m_rcLinePadding.top = m_rcLinePadding.bottom = 0;
	m_rcLinePadding.left = DEFAULT_LINE_LEFT_INSET;
	m_rcLinePadding.right = DEFAULT_LINE_RIGHT_INSET;
}

CMenuElementUI::~CMenuElementUI()
{
}

LPCTSTR CMenuElementUI::GetClass() const
{
	return _T("MenuElementUI");
}

LPVOID CMenuElementUI::GetInterface(LPCTSTR pstrName)
{
    if( _tcsicmp(pstrName, _T("MenuElement")) == 0 ) return static_cast<CMenuElementUI*>(this);    
    return CListContainerElementUI::GetInterface(pstrName);
}

void CMenuElementUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
    if( !::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem) ) return;

	if(m_bDrawLine)
	{
		int nFixedHeight = GetFixedHeight();
		RECT rcLine = { m_rcItem.left +  m_rcLinePadding.left, m_rcItem.top + nFixedHeight/2, m_rcItem.right - m_rcLinePadding.right, m_rcItem.top + nFixedHeight/2 };
		CRenderEngine::DrawLine(hDC, rcLine, 1, m_dwLineColor);
	}
	else
	{
		CMenuElementUI::DrawItemBk(hDC, m_rcItem);
		DrawItemText(hDC, m_rcItem);
		DrawItemIcon(hDC, m_rcItem);
		DrawItemExpland(hDC, m_rcItem);
		for (int i = 0; i < GetCount(); ++i)
		{
			if (GetItemAt(i)->GetInterface(_T("MenuElement")) == NULL)
				GetItemAt(i)->DoPaint(hDC, rcPaint);
		}
	}
}

void CMenuElementUI::DrawItemIcon(HDC hDC, const RECT& rcItem)
{
	if ( m_strIcon != _T("") )
	{
		
		if (!(m_bCheckItem && !GetChecked()))
		{
			CDuiString pStrImage;
			pStrImage.Format(_T("file='%s' dest='%d,%d,%d,%d'"), m_strIcon.GetData(), 
				(ITEM_DEFAULT_ICON_WIDTH - m_szIconSize.cx)/2,
				(GetFixedHeight() - m_szIconSize.cy)/2,
				(ITEM_DEFAULT_ICON_WIDTH - m_szIconSize.cx)/2 + m_szIconSize.cx,
				(GetFixedHeight() - m_szIconSize.cy)/2 + m_szIconSize.cy);
			DrawImage(hDC, pStrImage);
		}			
	}
}

void CMenuElementUI::DrawItemExpland(HDC hDC, const RECT& rcItem)
{
	if (m_bShowExplandIcon)
	{
		CDuiString strExplandIcon;
		strExplandIcon = GetManager()->GetDefaultAttributeList(_T("ExplandIcon"));
		CDuiString strBkImage;
		int nFixedWidth = GetFixedWidth();
		int nFixedHeight = GetFixedHeight();
		strBkImage.Format(_T("file='%s' dest='%d,%d,%d,%d'"), strExplandIcon.GetData(), 
			nFixedWidth - ITEM_DEFAULT_EXPLAND_ICON_WIDTH + (ITEM_DEFAULT_EXPLAND_ICON_WIDTH - ITEM_DEFAULT_EXPLAND_ICON_SIZE)/2,
			(nFixedHeight - ITEM_DEFAULT_EXPLAND_ICON_SIZE)/2,
			nFixedWidth - ITEM_DEFAULT_EXPLAND_ICON_WIDTH + (ITEM_DEFAULT_EXPLAND_ICON_WIDTH - ITEM_DEFAULT_EXPLAND_ICON_SIZE)/2 + ITEM_DEFAULT_EXPLAND_ICON_SIZE,
			(nFixedHeight - ITEM_DEFAULT_EXPLAND_ICON_SIZE)/2 + ITEM_DEFAULT_EXPLAND_ICON_SIZE);

		DrawImage(hDC, strBkImage);
	}
}


void CMenuElementUI::DrawItemText(HDC hDC, const RECT& rcItem)
{
    if( m_sText.IsEmpty() ) return;

    if( m_pOwner == NULL ) return;
    TListInfoUI* pInfo = m_pOwner->GetListInfo();
	DWORD iTextColor = GetTextColor();
    if( (m_uButtonState & UISTATE_HOT) != 0 ) {
        iTextColor = pInfo->dwHotTextColor;
    }
    if( IsSelected() ) {
        iTextColor = pInfo->dwSelectedTextColor;
    }
    if( !IsEnabled() ) {
        iTextColor = pInfo->dwDisabledTextColor;
    }
    int nLinks = 0;
    RECT rcText = rcItem;
    rcText.left += pInfo->rcTextPadding.left;
    rcText.right -= pInfo->rcTextPadding.right;
    rcText.top += pInfo->rcTextPadding.top;
    rcText.bottom -= pInfo->rcTextPadding.bottom;

	UINT uTextStyle = DT_SINGLELINE | pInfo->uTextStyle;
	if (m_uTextStyle != (DT_LEFT|DT_VCENTER))
	{
		uTextStyle = DT_SINGLELINE | m_uTextStyle;
	}
    if( pInfo->bShowHtml )
        CRenderEngine::DrawHtmlText(hDC, m_pManager, rcText, m_sText, iTextColor, \
        NULL, NULL, nLinks, uTextStyle);
    else
        CRenderEngine::DrawText(hDC, m_pManager, rcText, m_sText, iTextColor, \
        pInfo->sFont, uTextStyle);
}


SIZE CMenuElementUI::EstimateSize(SIZE szAvailable)
{
	double S = m_pManager ? m_pManager->GetDpiScale() : 1.0;
	if (!m_pOwner)
	{
		szAvailable.cx = szAvailable.cy = -1;
		return szAvailable;
	}
	if (S != 1.0 && (GetFixedWidth() == ITEM_DEFAULT_WIDTH || GetFixedHeight() == ITEM_DEFAULT_HEIGHT))
	{
		m_attrs.SetAttribute(DUI_ATTR_WIDTH, ITEM_DEFAULT_WIDTH, true);
		m_attrs.SetAttribute(DUI_ATTR_HEIGHT, ITEM_DEFAULT_HEIGHT, true);
	}

	SIZE cXY = {0};
	for( int it = 0; it < GetCount(); it++ ) {
		CControlUI* pControl = static_cast<CControlUI*>(GetItemAt(it));
		if( !pControl->IsVisible() ) continue;
		if (pControl->GetInterface(_T("MenuElement")) == NULL)
		{
			SIZE sz = pControl->EstimateSize(szAvailable);
			cXY.cy += sz.cy;
			if( cXY.cx < sz.cx )
				cXY.cx = sz.cx;
		}
	}
	if(cXY.cy == 0) {
		TListInfoUI* pInfo = m_pOwner->GetListInfo();

		DWORD iTextColor = pInfo->dwTextColor;
		if( (m_uButtonState & UISTATE_HOT) != 0 ) {
			iTextColor = pInfo->dwHotTextColor;
		}
		if( IsSelected() ) {
			iTextColor = pInfo->dwSelectedTextColor;
		}
		if( !IsEnabled() ) {
			iTextColor = pInfo->dwDisabledTextColor;
		}

		RECT rcText = { 0, 0, MAX(szAvailable.cx, GetFixedWidth()), 9999 };
		rcText.left += pInfo->rcTextPadding.left;
		rcText.right -= pInfo->rcTextPadding.right;
		if( pInfo->bShowHtml ) {   
			int nLinks = 0;
			CRenderEngine::DrawHtmlText(m_pManager->GetPaintDC(), m_pManager, rcText, m_sText, iTextColor, NULL, NULL, nLinks, DT_CALCRECT | pInfo->uTextStyle);
		}
		else
		{
			CRenderEngine::DrawText(m_pManager->GetPaintDC(), m_pManager, rcText, m_sText, iTextColor, pInfo->sFont, DT_CALCRECT | pInfo->uTextStyle);
		}
		cXY.cx = rcText.right - rcText.left + pInfo->rcTextPadding.left + pInfo->rcTextPadding.right + 20;
		cXY.cy = rcText.bottom - rcText.top + pInfo->rcTextPadding.top + pInfo->rcTextPadding.bottom;
	}

	if( GetFixedHeight() != 0 ) cXY.cy = GetFixedHeight();
	if ( cXY.cx < GetFixedWidth() )
		cXY.cx = GetFixedWidth();

	m_attrs.SetAttribute(DUI_ATTR_WIDTH, cXY.cx, false);
	m_attrs.SetAttribute(DUI_ATTR_HEIGHT, cXY.cy, false);
	return cXY;
}

void CMenuElementUI::DoEvent(TEventUI& event)
{

	if( event.Type == UIEVENT_MOUSEENTER )
	{
		CListContainerElementUI::DoEvent(event);
		if( m_pWindow ) return;
		bool hasSubMenu = false;
		for( int i = 0; i < GetCount(); ++i )
		{
			if( GetItemAt(i)->GetInterface(_T("MenuElement")) != NULL )
			{
				(static_cast<CMenuElementUI*>(GetItemAt(i)->GetInterface(_T("MenuElement"))))->SetVisible(true);
				(static_cast<CMenuElementUI*>(GetItemAt(i)->GetInterface(_T("MenuElement"))))->SetInternVisible(true);

				hasSubMenu = true;
			}
		}
		if( hasSubMenu )
		{
			m_pOwner->SelectItem(GetIndex(), true);
			CreateMenuWnd();
		}
		else
		{
			ContextMenuParam param;
			param.hWnd = m_pManager->GetPaintWindow();
			param.wParam = 2;
			CMenuWnd::GetGlobalContextMenuObserver().RBroadcast(param);
			m_pOwner->SelectItem(GetIndex(), true);
		}
		return;
	}

	if( event.Type == UIEVENT_BUTTONUP || ( event.Type == UIEVENT_KEYDOWN && event.chKey == VK_SPACE))
	{
		if( IsEnabled() ){
			CListContainerElementUI::DoEvent(event);

			if( m_pWindow ) return;

			bool hasSubMenu = false;
			for( int i = 0; i < GetCount(); ++i ) {
				if( GetItemAt(i)->GetInterface(_T("MenuElement")) != NULL ) {
					(static_cast<CMenuElementUI*>(GetItemAt(i)->GetInterface(_T("MenuElement"))))->SetVisible(true);
					(static_cast<CMenuElementUI*>(GetItemAt(i)->GetInterface(_T("MenuElement"))))->SetInternVisible(true);

					hasSubMenu = true;
				}
			}
			if( hasSubMenu )
			{
				CreateMenuWnd();
			}
			else
			{
				SetChecked(!GetChecked());
				if (CMenuWnd::GetGlobalContextMenuObserver().GetManager() != NULL)
				{
					LPDUIMENUITEMINFO lpMenuItemInfo = new DUIMENUITEMINFO;
					ASSERT(lpMenuItemInfo);
					lpMenuItemInfo->strName = GetName();
					lpMenuItemInfo->strUserData = GetUserData();
					lpMenuItemInfo->bChecked = GetChecked();
					//DoLuaEvent(DUI_MSGTYPE_CLICK, 0, 0);
					if (!PostMessage(CMenuWnd::GetGlobalContextMenuObserver().GetManager()->GetPaintWindow(), WM_MENUCLICK, (WPARAM)lpMenuItemInfo, 0))
					{
						delete lpMenuItemInfo;
						lpMenuItemInfo = NULL;
					}
				}
				ContextMenuParam param;
				param.hWnd = m_pManager->GetPaintWindow();
				param.wParam = 1;
				CMenuWnd::GetGlobalContextMenuObserver().RBroadcast(param);
			}
        }

        return;
    }

	if ( event.Type == UIEVENT_KEYDOWN && event.chKey == VK_RIGHT )
	{
		if( m_pWindow ) return;
		bool hasSubMenu = false;
		for( int i = 0; i < GetCount(); ++i )
		{
			if( GetItemAt(i)->GetInterface(_T("MenuElement")) != NULL )
			{
				(static_cast<CMenuElementUI*>(GetItemAt(i)->GetInterface(_T("MenuElement"))))->SetVisible(true);
				(static_cast<CMenuElementUI*>(GetItemAt(i)->GetInterface(_T("MenuElement"))))->SetInternVisible(true);

				hasSubMenu = true;
			}
		}
		if( hasSubMenu )
		{
			m_pOwner->SelectItem(GetIndex(), true);
			CreateMenuWnd();
		}
		else
		{
			ContextMenuParam param;
			param.hWnd = m_pManager->GetPaintWindow();
			param.wParam = 2;
			CMenuWnd::GetGlobalContextMenuObserver().RBroadcast(param);
			m_pOwner->SelectItem(GetIndex(), true);
		}

		return;
	}

    CListContainerElementUI::DoEvent(event);
}

CMenuWnd* CMenuElementUI::GetMenuWnd()
{
	return m_pWindow;
}

void CMenuElementUI::CreateMenuWnd()
{
	if( m_pWindow ) return;

	m_pWindow = new CMenuWnd();
	ASSERT(m_pWindow);

	ContextMenuParam param;
	param.hWnd = m_pManager->GetPaintWindow();
	param.wParam = 2;
	CMenuWnd::GetGlobalContextMenuObserver().RBroadcast(param);

	m_pWindow->Init(this, _T(""), CDuiPoint(), NULL);
}

void CMenuElementUI::SetLineType()
{
	m_bDrawLine = true;
	if (GetFixedHeight() == 0 || GetFixedHeight() == ITEM_DEFAULT_HEIGHT )
		SetFixedHeight(DEFAULT_LINE_HEIGHT);

	SetMouseChildEnabled(false);
	SetMouseEnabled(false);
	SetEnabled(false);
}

void CMenuElementUI::SetLineColor(DWORD color)
{
	m_dwLineColor = color;
}

DWORD CMenuElementUI::GetLineColor() const
{
	return m_dwLineColor;
}
void CMenuElementUI::SetLinePadding(RECT rcInset)
{
	m_rcLinePadding = rcInset;
}

RECT CMenuElementUI::GetLinePadding() const
{
	return m_rcLinePadding;
}

void CMenuElementUI::SetIcon(LPCTSTR strIcon)
{
	if ( strIcon != _T("") )
		m_strIcon = strIcon;
}

void CMenuElementUI::SetIconSize(LONG cx, LONG cy)
{
	m_szIconSize.cx = cx;
	m_szIconSize.cy = cy;
}

void CMenuElementUI::SetChecked(bool bCheck/* = true*/)
{
	if (!m_bCheckItem || CMenuWnd::GetGlobalContextMenuObserver().GetMenuCheckInfo() == NULL )
		return;
	std::map<CDuiString,bool>::iterator it = CMenuWnd::GetGlobalContextMenuObserver().GetMenuCheckInfo()->find(GetName());
	if (it == CMenuWnd::GetGlobalContextMenuObserver().GetMenuCheckInfo()->end())
		CMenuWnd::GetGlobalContextMenuObserver().GetMenuCheckInfo()->insert(std::map<CDuiString,bool>::value_type(GetName(),bCheck));
	else
		it->second = bCheck;

}

bool CMenuElementUI::GetChecked() const
{

	if (!m_bCheckItem || CMenuWnd::GetGlobalContextMenuObserver().GetMenuCheckInfo() == NULL || CMenuWnd::GetGlobalContextMenuObserver().GetMenuCheckInfo()->size() == 0)
		return false;

	std::map<CDuiString,bool>::iterator it = CMenuWnd::GetGlobalContextMenuObserver().GetMenuCheckInfo()->find(GetName());
	if (it != CMenuWnd::GetGlobalContextMenuObserver().GetMenuCheckInfo()->end())
	{
		return it->second;
	}
	return false;

}

void CMenuElementUI::SetCheckItem(bool bCheckItem/* = false*/)
{
	m_bCheckItem = bCheckItem;
}

bool CMenuElementUI::GetCheckItem() const
{
	return m_bCheckItem;
}

void CMenuElementUI::SetShowExplandIcon(bool bShow)
{
	m_bShowExplandIcon = bShow;
}

void CMenuElementUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	double S = m_pManager ? m_pManager->GetDpiScale() : 1.0;

	if( _tcsicmp(pstrName, _T("icon")) == 0){
		SetIcon(pstrValue);
	}
	else if( _tcsicmp(pstrName, _T("iconsize")) == 0 ) {
		LPTSTR pstr = NULL;
		LONG cx = 0, cy = 0;
		cx = _tcstol(pstrValue, &pstr, 10)*S;  ASSERT(pstr);    
		cy = _tcstol(pstr + 1, &pstr, 10)*S;    ASSERT(pstr);
		SetIconSize(cx, cy);
	}
	else if( _tcsicmp(pstrName, _T("checkitem")) == 0 ) {		
		SetCheckItem(_tcsicmp(pstrValue, _T("true")) == 0 ? true : false);		
	}
	else if( _tcsicmp(pstrName, _T("ischeck")) == 0 ) {		
		if (CMenuWnd::GetGlobalContextMenuObserver().GetMenuCheckInfo() != NULL && CMenuWnd::GetGlobalContextMenuObserver().GetMenuCheckInfo()->find(GetName()) == CMenuWnd::GetGlobalContextMenuObserver().GetMenuCheckInfo()->end())
		{
			SetChecked(_tcsicmp(pstrValue, _T("true")) == 0 ? true : false);
		}	
	}	
	else if( _tcsicmp(pstrName, _T("linetype")) == 0){
		if (_tcsicmp(pstrValue, _T("true")) == 0)
			SetLineType();
	}
	else if( _tcsicmp(pstrName, _T("expland")) == 0 ) {
		SetShowExplandIcon(_tcsicmp(pstrValue, _T("true")) == 0 ? true : false);
	}
	else if( _tcsicmp(pstrName, _T("linecolor")) == 0){
		if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
		LPTSTR pstr = NULL;
		SetLineColor(_tcstoul(pstrValue, &pstr, 16));
	}
	else if( _tcsicmp(pstrName, _T("linepadding")) == 0 ) {
		RECT rcInset = { 0 };
		LPTSTR pstr = NULL;
		rcInset.left = _tcstol(pstrValue, &pstr, 10)*S;  ASSERT(pstr);
		rcInset.top = _tcstol(pstr + 1, &pstr, 10)*S;    ASSERT(pstr);
		rcInset.right = _tcstol(pstr + 1, &pstr, 10)*S;  ASSERT(pstr);
		rcInset.bottom = _tcstol(pstr + 1, &pstr, 10)*S; ASSERT(pstr);
		SetLinePadding(rcInset);
	}
	else if	( _tcsicmp(pstrName, _T("height")) == 0){
		SetFixedHeight(_ttoi(pstrValue)*S);
	}
	else if( _tcsicmp(pstrName, _T("align")) == 0 ) {
		if( _tcsstr(pstrValue, _T("left")) != NULL ) {
			m_uTextStyle &= ~(DT_CENTER | DT_RIGHT);
			m_uTextStyle |= DT_LEFT;
		}
		if( _tcsstr(pstrValue, _T("center")) != NULL ) {
			m_uTextStyle &= ~(DT_LEFT | DT_RIGHT );
			m_uTextStyle |= DT_CENTER;
		}
		if( _tcsstr(pstrValue, _T("right")) != NULL ) {
			m_uTextStyle &= ~(DT_LEFT | DT_CENTER);
			m_uTextStyle |= DT_RIGHT;
		}
	}
	else if( _tcsicmp(pstrName, _T("valign")) == 0 ) {
		if( _tcsstr(pstrValue, _T("top")) != NULL ) {
			m_uTextStyle &= ~(DT_BOTTOM | DT_VCENTER);
			m_uTextStyle |= (DT_TOP | DT_SINGLELINE);
		}
		if( _tcsstr(pstrValue, _T("vcenter")) != NULL ) {
			m_uTextStyle &= ~(DT_TOP | DT_BOTTOM );            
			m_uTextStyle |= (DT_VCENTER | DT_SINGLELINE);
		}
		if( _tcsstr(pstrValue, _T("bottom")) != NULL ) {
			m_uTextStyle &= ~(DT_TOP | DT_VCENTER);
			m_uTextStyle |= (DT_BOTTOM | DT_SINGLELINE);
		}
	}
	else
		CListContainerElementUI::SetAttribute(pstrName, pstrValue);
}

} // namespace DuiLib
