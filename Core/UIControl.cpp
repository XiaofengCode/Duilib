#include "duipub.h"
///////////////////////////////////////////////////////////////////////////////////////

namespace DuiLib {

CControlUI::CControlUI() : 
m_pManager(NULL), 
m_pParent(NULL), 
m_bUpdateNeeded(true),
m_bMenuUsed(false),
m_bVisible(true), 
m_bInternVisible(true),
m_bFocused(false),
m_bEnabled(true),
m_bMouseEnabled(true),
m_bKeyboardEnabled(true),
m_bFloat(false),
m_bSetPos(false),
m_chShortcut('\0'),
m_pTag(NULL),
m_bColorHSL(false),
m_nBorderSize(0),
m_nBorderStyle(PS_SOLID),
m_nTooltipWidth(300)
{
//	m_attrs.AddKeyword(DUI_ATTR_NAME, TypeString);

	m_attrs.AddKeyword(DUI_ATTR_COLOR, TypeColor);
	m_attrs.AddKeyword(DUI_ATTR_COLOR2, TypeColor);
	m_attrs.AddKeyword(DUI_ATTR_COLOR3, TypeColor);
	m_attrs.AddKeyword(DUI_ATTR_POS, TypeRect);
	m_attrs.AddKeyword(DUI_ATTR_PADDING, TypeRect);
	m_attrs.AddKeyword(DUI_ATTR_SIZE, TypeIntOrRect);
	m_attrs.AddKeyword(DUI_ATTR_ROUND, TypeRect);
	m_attrs.AddKeyword(DUI_ATTR_WIDTH, TypeInt);
	m_attrs.AddKeyword(DUI_ATTR_HEIGHT, TypeInt);
	m_attrs.AddKeyword(DUI_ATTR_FONT, TypeString);
	m_attrs.AddKeyword(DUI_ATTR_TOOLTIP, TypeString);
	m_attrs.AddKeyword(DUI_ATTR_IMAGE, TypeImage);
	m_attrs.AddKeyword(DUI_ATTR_TIPVALUE, TypeString);
// 	m_attrs.AddKeyword(DUI_ATTR_USERDATA, TypeString);
// 	m_attrs.AddKeyword(DUI_ATTR_ENABLED, TypeBool);
// 	m_attrs.AddKeyword(DUI_ATTR_KEYBOARD, TypeBool);
// 	m_attrs.AddKeyword(DUI_ATTR_VISIBLE, TypeBool);
// 	m_attrs.AddKeyword(DUI_ATTR_FLOAT, TypeBool);
// 	m_attrs.AddKeyword(DUI_ATTR_SHORTCUT, TypeString);
// 	m_attrs.AddKeyword(DUI_ATTR_MENU, TypeBool);
//	m_attrs.AddKeyword(DUI_ATTR_VIRTUALWND, TypeString);

	m_attrs.AddKeyword(DUI_ATTR_TEXT, TypeString);

	m_attrs.AddKeyword(DUI_ATTR_POS_FOCUSDOT);
	m_attrs.AddKeyword(DUI_ATTR_POS_BK);
	m_attrs.AddKeyword(DUI_ATTR_POS_FORE);
	m_attrs.AddKeyword(DUI_ATTR_POS_NORMAL);
	m_attrs.AddKeyword(DUI_ATTR_POS_BORDER);

	m_attrs.AddKeyword(DUI_ATTR_STATUS_FOCUSED);
	m_attrs.AddKeyword(DUI_ATTR_STATUS_DISABLED);
	m_attrs.AddKeyword(DUI_ATTR_STATUS_HOT);
	m_attrs.AddKeyword(DUI_ATTR_STATUS_PUSHED);
	m_attrs.AddKeyword(DUI_ATTR_STATUS_SELECTED);

	m_cXY.cx = m_cXY.cy = 0;
	m_attrs.SetAttribute(DUI_ATTR_WIDTH, 0);
	m_attrs.SetAttribute(DUI_ATTR_HEIGHT, 0);
    m_cxyMin.cx = m_cxyMin.cy = 0;
    m_cxyMax.cx = m_cxyMax.cy = 9999;
    m_cxyBorderRound.cx = m_cxyBorderRound.cy = 0;

    ::ZeroMemory(&m_rcPadding, sizeof(RECT));
    ::ZeroMemory(&m_rcItem, sizeof(RECT));
    ::ZeroMemory(&m_rcPaint, sizeof(RECT));
	::ZeroMemory(&m_rcBorderSize,sizeof(RECT));
    ::ZeroMemory(&m_tRelativePos, sizeof(TRelativePosUI));

	m_nWidthScale = 0;
	m_nHeightScale = 0;
}

CControlUI::~CControlUI()
{
    if( OnDestroy ) OnDestroy(this);
    if( m_pManager != NULL ) m_pManager->ReapObjects(this);
}

bool CControlUI::DoLuaEvent(LPCTSTR evName,LuaObject wParam, LuaObject lParam, LuaObject event)
{
	ASSERT(evName);
	if (GetManager())
	{
		LuaState* L = GetManager()->GetLuaState();
		//LOGI("DoLuaEvent:"<<evName);
		LuaTable tab=GetManager()->GetControlEventMap(this,false);
		if (tab.isValid() && L)
		{
			LuaObject evData=tab.getTable(DUI_T2A(evName).c_str());
			if (evData.isFunction())
			{
				LuaFunction func=evData;
				try
				{
					if (event.toInt())
					{
						LuaObject rtn=func(_lbindCToLua(L), wParam, lParam, event);
						return rtn.toBool();
					}
					else
					{
						LuaObject rtn=func(_lbindCToLua(L), wParam, lParam);
						return rtn.toBool();
					}
				}
				catch(LuaException err)
				{
					OutputDebugStringA(err.what());
					//LOGE("exec function error:"<<err.what());
				}
				return false;
			}
			else if(evData.isString())
			{
				try{
					LuaObject lthis=_lbindCToLua(L);
					L->setGlobal("this",lthis);
					L->setGlobal("wParam",wParam);
					L->setGlobal("lParam",lParam);
					L->setGlobal("nEvent",event);
					const char* script=evData.toString();
					L->doString(script);
				}
				catch(LuaException err)
				{
					OutputDebugStringA(err.what());
					//LOGE("doString error:"<<err.what());
				}
			}
		}
	}
	return false;
}

bool CControlUI::DoLuaEvent(LPCTSTR evName, DWORD wParam, DWORD lParam, DWORD nEvent/* = 0*/)
{
	if (!GetManager())
	{
		return false;
	}
	LuaState* L=GetManager()->GetLuaState();
	if (L)
	{
		//LOGI("DoLuaEvent:"<<evName);
		return DoLuaEvent(evName, L->newInt(wParam),L->newInt(lParam), L->newInt(nEvent));
	}
	return false;
}

void CControlUI::BindLuaEvent(LPCTSTR evName,LuaObject func)
{
	ASSERT(evName);
	if (GetManager() && func.isFunction())
	{
		LuaTable tab=GetManager()->GetControlEventMap(this,true);
		ASSERT(tab.isValid());
		CDuiString strEvent(evName);
		strEvent.MakeLower();
		tab.setTable(DUI_T2A(strEvent).c_str(), func);
	}
}

void CControlUI::BindLuaEvent(LPCTSTR evName,LPCTSTR luaSrc)
{
	ASSERT(evName);
	if (GetManager())
	{
		LuaTable tab=GetManager()->GetControlEventMap(this,true);
		ASSERT(tab.isValid());
		CDuiString strEvent(evName);
		strEvent.MakeLower();
		tab.setTable(DUI_T2A(strEvent).c_str(), DUI_T2A(luaSrc).c_str());
	}
}

CDuiString CControlUI::GetName() const
{
    return m_sName;
}

void CControlUI::SetName(LPCTSTR pstrName)
{
    m_sName = pstrName;
}

LPVOID CControlUI::GetInterface(LPCTSTR pstrName)
{
    if( _tcscmp(pstrName, DUI_CTR_CONTROL) == 0 ) return this;
    return NULL;
}

LPCTSTR CControlUI::GetClass() const
{
    return _T("ControlUI");
}

UINT CControlUI::GetControlFlags() const
{
    return 0;
}

bool CControlUI::Activate()
{
    if( !IsVisible() ) return false;
    if( !IsEnabled() ) return false;
    return true;
}

CPaintManagerUI* CControlUI::GetManager() const
{
    return m_pManager;
}

void CControlUI::SetManager(CPaintManagerUI* pManager, CControlUI* pParent, bool bInit)
{
    m_pManager = pManager;
    m_pParent = (CContainerUI*)pParent;
    if( bInit && m_pParent ) Init();
}

CContainerUI* CControlUI::GetParent() const
{
    return m_pParent;
}

CDuiString CControlUI::GetText() const
{
    return m_sText;
}

void CControlUI::SetText(LPCTSTR pstrText)
{
    if( m_sText == pstrText ) return;

    m_sText = pstrText;
    Invalidate();
}

double CControlUI::GetScaleDpi()
{
	double S = 1.0;
	if (m_pManager)
	{
		S = m_pManager->GetDpiScale();
	}

	return S;
}

void CControlUI::SetTextColor(DWORD dwTextColor)
{
	if( GetTextColor() == dwTextColor ) return;

	m_attrs.SetAttribute(DUI_ATTR_TEXT DUI_ATTR_COLOR, dwTextColor);
	Invalidate();
}

DWORD CControlUI::GetTextColor() const
{
	return m_attrs.GetColor(DUI_ATTR_TEXT DUI_ATTR_COLOR);
}

DWORD CControlUI::GetBkColor() const
{
	return m_attrs.GetColor(DUI_ATTR_POS_BK DUI_ATTR_COLOR);
}

void CControlUI::SetBkColor(DWORD dwBackColor)
{
	if( GetBkColor() == dwBackColor ) return;

	m_attrs.SetAttribute(DUI_ATTR_POS_BK DUI_ATTR_COLOR, dwBackColor);
    Invalidate();
}

DWORD CControlUI::GetBkColor2() const
{
	return m_attrs.GetColor(DUI_ATTR_POS_BK DUI_ATTR_COLOR2);
}

void CControlUI::SetBkColor2(DWORD dwBackColor)
{
	if( GetBkColor2() == dwBackColor ) return;

	m_attrs.SetAttribute(DUI_ATTR_POS_BK DUI_ATTR_COLOR2, dwBackColor);
	Invalidate();
}

DWORD CControlUI::GetBkColor3() const
{
	return m_attrs.GetColor(DUI_ATTR_POS_BK DUI_ATTR_COLOR3);
}

void CControlUI::SetBkColor3(DWORD dwBackColor)
{
	if( GetBkColor3() == dwBackColor ) return;

	m_attrs.SetAttribute(DUI_ATTR_POS_BK DUI_ATTR_COLOR3, dwBackColor);
	Invalidate();
}

const CDuiImage CControlUI::GetBkImage()
{
	return m_attrs.GetImage(DUI_ATTR_POS_BK DUI_ATTR_IMAGE);
}

void CControlUI::SetBkImage(LPCTSTR pStrImage)
{
	CDuiString sImageOld = m_attrs.GetString(DUI_ATTR_POS_BK DUI_ATTR_IMAGE);
    if( sImageOld == pStrImage )
		return;

	if (!m_attrs.SetAttribute(DUI_ATTR_POS_BK DUI_ATTR_IMAGE, pStrImage))
		return;

	const CDuiImage img = GetBkImage();
	if (img)
	{
		m_gifBk.Stop();
		m_gifBk.DeleteImage();
		m_gifBk.InitImage(this, img[0].m_strFile);

		Invalidate();
	}
}

DWORD CControlUI::GetBorderColor() const
{
	return m_attrs.GetColor(DUI_ATTR_POS_BORDER DUI_ATTR_COLOR);
}

void CControlUI::SetBorderColor(DWORD dwBorderColor)
{
	if( GetBorderColor() == dwBorderColor ) return;

	m_attrs.SetAttribute(DUI_ATTR_POS_BORDER DUI_ATTR_COLOR, dwBorderColor);
	Invalidate();
}

DWORD CControlUI::GetFocusDotColor() const
{
	return m_attrs.GetColor(DUI_ATTR_POS_FOCUSDOT DUI_ATTR_COLOR);
}

void CControlUI::SetFocusDotColor(DWORD dwColor)
{
	if( GetFocusDotColor() == dwColor ) return;

	m_attrs.SetAttribute(DUI_ATTR_POS_FOCUSDOT DUI_ATTR_COLOR, dwColor);
	Invalidate();
}

DWORD CControlUI::GetFocusBorderColor() const
{
	return m_attrs.GetColor(DUI_ATTR_STATUS_FOCUSED DUI_ATTR_POS_BORDER DUI_ATTR_COLOR);
}

void CControlUI::SetFocusBorderColor(DWORD dwBorderColor)
{
	if( GetFocusBorderColor() == dwBorderColor ) return;

	m_attrs.SetAttribute(DUI_ATTR_STATUS_FOCUSED DUI_ATTR_POS_BORDER DUI_ATTR_COLOR, dwBorderColor);
	Invalidate();
}

DWORD CControlUI::GetDisabledBorderColor() const
{
	return m_attrs.GetColor(DUI_ATTR_STATUS_DISABLED DUI_ATTR_POS_BORDER DUI_ATTR_COLOR);
}

void CControlUI::SetDisabledBorderColor(DWORD dwBorderColor)
{
	if( GetDisabledBorderColor() == dwBorderColor ) return;

	m_attrs.SetAttribute(DUI_ATTR_STATUS_DISABLED DUI_ATTR_POS_BORDER DUI_ATTR_COLOR, dwBorderColor);
	Invalidate();
}

bool CControlUI::IsColorHSL() const
{
    return m_bColorHSL;
}

void CControlUI::SetColorHSL(bool bColorHSL)
{
    if( m_bColorHSL == bColorHSL ) return;

    m_bColorHSL = bColorHSL;
    Invalidate();
}

int CControlUI::GetBorderSize() const
{
    return m_nBorderSize;
}

void CControlUI::SetBorderSize(int nSize)
{
    if( m_nBorderSize == nSize ) return;

    m_nBorderSize = nSize;
    Invalidate();
}

void CControlUI::SetBorderSize( RECT rc )
{
	m_rcBorderSize = rc;
	Invalidate();
}

SIZE CControlUI::GetBorderRound() const
{
    return m_cxyBorderRound;
}

void CControlUI::SetBorderRound(SIZE cxyRound)
{
    m_cxyBorderRound = cxyRound;
    Invalidate();
}

bool CControlUI::DrawImage(HDC hDC, LPCTSTR pStrImage, LPCTSTR pStrModify)
{
    return CRenderEngine::DrawImageStringX(hDC, m_pManager, m_rcItem, m_rcPaint, pStrImage, pStrModify);
}

bool CControlUI::DrawImage(HDC hDC, const CDuiImageItem& img)
{
	return CRenderEngine::DrawImage(hDC, m_pManager, m_rcItem, m_rcPaint, img);
}

bool CControlUI::DrawImage(HDC hDC, const CDuiImage& img)
{
	return CRenderEngine::DrawImage(hDC, m_pManager, m_rcItem, m_rcPaint, img);
}

const RECT& CControlUI::GetPos() const
{
    return m_rcItem;
}

void CControlUI::SetPos(RECT rc)
{
    if( rc.right < rc.left ) rc.right = rc.left;
    if( rc.bottom < rc.top ) rc.bottom = rc.top;

    CDuiRect invalidateRc = m_rcItem;
    if( ::IsRectEmpty(&invalidateRc) ) invalidateRc = rc;

    m_rcItem = rc;
    if( m_pManager == NULL ) return;

    if( !m_bSetPos ) {
        m_bSetPos = true;
        if( OnSize ) OnSize(this);
        m_bSetPos = false;
    }
    
    if( m_bFloat ) {
        CControlUI* pParent = GetParent();
        if( pParent != NULL ) {
            RECT rcParentPos = pParent->GetPos();
            if( m_cXY.cx >= 0 ) m_cXY.cx = m_rcItem.left - rcParentPos.left;
            else m_cXY.cx = m_rcItem.right - rcParentPos.right;
            if( m_cXY.cy >= 0 ) m_cXY.cy = m_rcItem.top - rcParentPos.top;
			else m_cXY.cy = m_rcItem.bottom - rcParentPos.bottom;
			m_attrs.SetAttribute(DUI_ATTR_WIDTH, m_rcItem.right - m_rcItem.left);
			m_attrs.SetAttribute(DUI_ATTR_HEIGHT, m_rcItem.bottom - m_rcItem.top);
        }
    }

    m_bUpdateNeeded = false;
    invalidateRc.Join(m_rcItem);

    CControlUI* pParent = this;
    RECT rcTemp;
    RECT rcParent;
    while( pParent = pParent->GetParent() )
    {
        rcTemp = invalidateRc;
        rcParent = pParent->GetPos();
        if( !::IntersectRect(&invalidateRc, &rcTemp, &rcParent) ) 
        {
            return;
        }
    }
    m_pManager->Invalidate(invalidateRc);
}

int CControlUI::GetWidth() const
{
    return m_rcItem.right - m_rcItem.left;
}

int CControlUI::GetHeight() const
{
    return m_rcItem.bottom - m_rcItem.top;
}

int CControlUI::GetX() const
{
    return m_rcItem.left;
}

int CControlUI::GetY() const
{
    return m_rcItem.top;
}

RECT CControlUI::GetPadding() const
{
    return m_rcPadding;
}

void CControlUI::SetPadding(RECT rcPadding)
{
    m_rcPadding = rcPadding;
    NeedParentUpdate();
}

SIZE CControlUI::GetFixedXY() const
{
    return m_cXY;
}

void CControlUI::SetFixedXY(SIZE szXY)
{
    m_cXY.cx = szXY.cx;
    m_cXY.cy = szXY.cy;
    if( !m_bFloat ) NeedParentUpdate();
    else NeedUpdate();
}

int CControlUI::GetFixedWidth() const
{
	return m_attrs.GetInt(DUI_ATTR_WIDTH);
}

void CControlUI::SetFixedWidth(int cx)
{
	if( cx < 0 ) return; 
	m_attrs.SetAttribute(DUI_ATTR_WIDTH, cx);
    if( !m_bFloat ) NeedParentUpdate();
    else NeedUpdate();
}

int CControlUI::GetFixedHeight() const
{
	return m_attrs.GetInt(DUI_ATTR_HEIGHT);
}

void CControlUI::SetFixedHeight(int cy)
{
	if( cy < 0 ) return; 
	m_attrs.SetAttribute(DUI_ATTR_HEIGHT, cy);
    if( !m_bFloat ) NeedParentUpdate();
    else NeedUpdate();
}

int CControlUI::GetMinWidth() const
{
    return m_cxyMin.cx;
}

void CControlUI::SetMinWidth(int cx)
{
    if( m_cxyMin.cx == cx ) return;

    if( cx < 0 ) return; 
    m_cxyMin.cx = cx;
    if( !m_bFloat ) NeedParentUpdate();
    else NeedUpdate();
}

int CControlUI::GetMaxWidth() const
{
    return m_cxyMax.cx;
}

void CControlUI::SetMaxWidth(int cx)
{
    if( m_cxyMax.cx == cx ) return;

    if( cx < 0 ) return; 
    m_cxyMax.cx = cx;
    if( !m_bFloat ) NeedParentUpdate();
    else NeedUpdate();
}

int CControlUI::GetMinHeight() const
{
    return m_cxyMin.cy;
}

void CControlUI::SetMinHeight(int cy)
{
    if( m_cxyMin.cy == cy ) return;

    if( cy < 0 ) return; 
    m_cxyMin.cy = cy;
    if( !m_bFloat ) NeedParentUpdate();
    else NeedUpdate();
}

int CControlUI::GetMaxHeight() const
{
    return m_cxyMax.cy;
}

void CControlUI::SetMaxHeight(int cy)
{
    if( m_cxyMax.cy == cy ) return;

    if( cy < 0 ) return; 
    m_cxyMax.cy = cy;
    if( !m_bFloat ) NeedParentUpdate();
    else NeedUpdate();
}

void CControlUI::SetRelativePos(SIZE szMove,SIZE szZoom)
{
    m_tRelativePos.bRelative = TRUE;
    m_tRelativePos.nMoveXPercent = szMove.cx;
    m_tRelativePos.nMoveYPercent = szMove.cy;
    m_tRelativePos.nZoomXPercent = szZoom.cx;
    m_tRelativePos.nZoomYPercent = szZoom.cy;
}

void CControlUI::SetRelativeParentSize(SIZE sz)
{
    m_tRelativePos.szParent = sz;
}

TRelativePosUI CControlUI::GetRelativePos() const
{
    return m_tRelativePos;
}

bool CControlUI::IsRelativePos() const
{
    return m_tRelativePos.bRelative;
}

CDuiString CControlUI::GetToolTip() const
{
    return m_sToolTip;
}

void CControlUI::SetToolTip(LPCTSTR pstrText)
{
	CDuiString strTemp(pstrText);
	strTemp.Replace(_T("<n>"),_T("\r\n"));
	m_sToolTip=strTemp;
}

void CControlUI::SetToolTipWidth( int nWidth )
{
	m_nTooltipWidth=nWidth;
}

int CControlUI::GetToolTipWidth( void )
{
	return m_nTooltipWidth;
}

TCHAR CControlUI::GetShortcut() const
{
    return m_chShortcut;
}

void CControlUI::SetShortcut(TCHAR ch)
{
    m_chShortcut = ch;
}

bool CControlUI::IsContextMenuUsed() const
{
    return m_bMenuUsed;
}

void CControlUI::SetContextMenuUsed(bool bMenuUsed)
{
    m_bMenuUsed = bMenuUsed;
}

const CDuiString& CControlUI::GetUserData()
{
    return m_sUserData;
}

void CControlUI::SetUserData(LPCTSTR pstrText)
{
    m_sUserData = pstrText;
}

UINT_PTR CControlUI::GetTag() const
{
    return m_pTag;
}

void CControlUI::SetTag(UINT_PTR pTag)
{
    m_pTag = pTag;
}

bool CControlUI::IsVisible() const
{

    return m_bVisible && m_bInternVisible;
}

void CControlUI::SetVisible(bool bVisible)
{
    if( m_bVisible == bVisible ) return;

    bool v = IsVisible();
    m_bVisible = bVisible;
    if( m_bFocused ) m_bFocused = false;
	if (!bVisible && m_pManager && m_pManager->GetFocus() == this) {
		m_pManager->SetFocus(NULL) ;
	}
    if( IsVisible() != v ) {
        NeedParentUpdate();
    }

	if (bVisible)
		m_gifBk.Play();
	else
		m_gifBk.Stop();
}

void CControlUI::SetInternVisible(bool bVisible)
{
    m_bInternVisible = bVisible;
	if (!bVisible && m_pManager && m_pManager->GetFocus() == this) {
		m_pManager->SetFocus(NULL) ;
	}
}

bool CControlUI::IsEnabled() const
{
    return m_bEnabled;
}

void CControlUI::SetEnabled(bool bEnabled)
{
    if( m_bEnabled == bEnabled ) return;

    m_bEnabled = bEnabled;
    Invalidate();
}

bool CControlUI::IsMouseEnabled() const
{
    return m_bMouseEnabled;
}

void CControlUI::SetMouseEnabled(bool bEnabled)
{
    m_bMouseEnabled = bEnabled;
}

bool CControlUI::IsKeyboardEnabled() const
{
	return m_bKeyboardEnabled ;
}

void CControlUI::SetKeyboardEnabled(bool bEnabled)
{
	m_bKeyboardEnabled = bEnabled ; 
}

bool CControlUI::IsFocused() const
{
    return m_bFocused;
}

void CControlUI::SetFocus()
{
    if( m_pManager != NULL ) m_pManager->SetFocus(this);
}

bool CControlUI::IsFloat() const
{
    return m_bFloat;
}

void CControlUI::SetFloat(bool bFloat)
{
    if( m_bFloat == bFloat ) return;

    m_bFloat = bFloat;
    NeedParentUpdate();
}

CControlUI* CControlUI::FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags)
{
    if( (uFlags & UIFIND_VISIBLE) != 0 && !IsVisible() ) return NULL;
    if( (uFlags & UIFIND_ENABLED) != 0 && !IsEnabled() ) return NULL;
	if( (uFlags & UIFIND_HITTEST) != 0 && (!m_bMouseEnabled || !::PtInRect(&m_rcItem, * static_cast<LPPOINT>(pData))) ) return NULL;
    return Proc(this, pData);
}

void CControlUI::Invalidate()
{
    if( !IsVisible() ) return;

    RECT invalidateRc = m_rcItem;

    CControlUI* pParent = this;
    RECT rcTemp;
    RECT rcParent;
    while( pParent = pParent->GetParent() )
    {
        rcTemp = invalidateRc;
        rcParent = pParent->GetPos();
        if( !::IntersectRect(&invalidateRc, &rcTemp, &rcParent) ) 
        {
            return;
        }
    }

    if( m_pManager != NULL ) m_pManager->Invalidate(invalidateRc);
}

bool CControlUI::IsUpdateNeeded() const
{
    return m_bUpdateNeeded;
}

void CControlUI::NeedUpdate()
{
    if( !IsVisible() ) return;
    m_bUpdateNeeded = true;
    Invalidate();

    if( m_pManager != NULL ) m_pManager->NeedUpdate();
}

void CControlUI::NeedParentUpdate()
{
    if( GetParent() ) {
        GetParent()->NeedUpdate();
        GetParent()->Invalidate();
    }
    else {
        NeedUpdate();
    }

    if( m_pManager != NULL ) m_pManager->NeedUpdate();
}

DWORD CControlUI::GetAdjustColor(DWORD dwColor)
{
    if( !m_bColorHSL ) return dwColor;
    short H, S, L;
    CPaintManagerUI::GetHSL(&H, &S, &L);
    return CRenderEngine::AdjustColor(dwColor, H, S, L);
}

void CControlUI::Init()
{
    DoInit();
    if( OnInit ) OnInit(this);
}

void CControlUI::DoInit()
{
	const CDuiImage img = GetBkImage();
	if (img)
	{
		m_gifBk.InitImage(this, img[0].m_strFile);
	}
}

void CControlUI::Event(TEventUI& event)
{
	DoLuaEvent(_T("preevent"), event.wParam, event.lParam, event.Type);
	if( OnEvent(&event) ) DoEvent(event);
	DoLuaEvent(_T("event"), event.wParam, event.lParam, event.Type);
	DoLuaEvent(_T("postevent"), event.wParam, event.lParam, event.Type);
}

void CControlUI::DoEvent(TEventUI& event)
{
    if( event.Type == UIEVENT_SETCURSOR )
    {
        ::SetCursor(::LoadCursor(NULL, IDC_ARROW));
        return;
    }
    if( event.Type == UIEVENT_SETFOCUS ) 
	{
		if (m_pParent)
		{
			RECT rcParent = m_pParent->GetPos();
			RECT rcInset = m_pParent->GetInset();
			rcParent.left += rcInset.left;
			rcParent.top += rcInset.top;
			rcParent.right -= rcInset.right;
			rcParent.bottom -= rcInset.bottom;
			int dx = 0;
			int dy = 0;
			if( m_rcItem.left < rcParent.left ) dx = m_rcItem.left - rcParent.left;
			if( m_rcItem.right > rcParent.right ) dx = m_rcItem.right - rcParent.right;
			if( m_rcItem.top < rcParent.top ) dy = m_rcItem.top - rcParent.top;
			if( m_rcItem.bottom > rcParent.bottom ) dy = m_rcItem.bottom - rcParent.bottom;
			if (dy|dx)
			{
				SIZE sz = m_pParent->GetScrollPos();
				sz.cx += dx;
				sz.cy += dy;
				m_pParent->SetScrollPos(sz);
			}
		}

        m_bFocused = true;
        Invalidate();
        return;
    }
    if( event.Type == UIEVENT_KILLFOCUS ) 
    {
        m_bFocused = false;
        Invalidate();
        return;
    }
    if( event.Type == UIEVENT_TIMER )
    {
		OnTimer( (UINT_PTR)event.wParam );
        m_pManager->SendNotify(this, DUI_MSGTYPE_TIMER, event.wParam, event.lParam);
        return;
    }
    if( event.Type == UIEVENT_CONTEXTMENU )
    {
        if( IsContextMenuUsed() ) {
            m_pManager->SendNotify(this, DUI_MSGTYPE_MENU, event.wParam, event.lParam);
            return;
        }
    }
    if( event.Type == UIEVENT_DBLCLICK )
    {
        m_pManager->SendNotify(this, DUI_MSGTYPE_DBCLICK, event.wParam, event.lParam);
        //return;
    }
    
    if( m_pParent != NULL ) m_pParent->DoEvent(event);
}


void CControlUI::SetVirtualWnd(LPCTSTR pstrValue)
{
	m_sVirtualWnd = pstrValue;
	m_pManager->UsedVirtualWnd(true);
}

CDuiString CControlUI::GetVirtualWnd(bool bGetParent/* = true*/) const
{
	CDuiString str;
	if( !m_sVirtualWnd.IsEmpty() )
	{
		str = m_sVirtualWnd;
	}
	else if (bGetParent)
	{
		CControlUI* pParent = GetParent();
		if( pParent != NULL)
		{
			str = pParent->GetVirtualWnd();
		}
		else
		{
			str = _T("");
		}
	}
	return str;
}

int CControlUI::GetVirtualWnd(CDuiStringArray& arVirtualWnd) const
{
	CDuiString str;
	arVirtualWnd.RemoveAll();
	if( !m_sVirtualWnd.IsEmpty() )
	{
		arVirtualWnd.Add(m_sVirtualWnd);
	}
	CControlUI* pParent = GetParent();
	if( pParent != NULL)
	{
		CDuiStringArray arVirtualWndParent;
		pParent->GetVirtualWnd(arVirtualWndParent);
		arVirtualWnd.Append(arVirtualWndParent);
	}
	return arVirtualWnd.GetSize();
}

CDuiString CControlUI::GetAttribute(LPCTSTR pstrName) const
{
	return m_attrs.GetString(pstrName);
}

void CControlUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	double S = 1.0;
	if (m_pManager)
	{
		S = m_pManager->GetDpiScale();
	}
	if( _tcsicmp(pstrName, _T("pos")) == 0 )
	{
        RECT rcPos = { 0 };
        LPTSTR pstr = NULL;
		rcPos.left = (LONG)(S*_tcstol(pstrValue, &pstr, 10));
		ASSERT(pstr);
		rcPos.top = (LONG)(S*_tcstol(pstr + 1, &pstr, 10));
		ASSERT(pstr);
		rcPos.right = (LONG)(S*_tcstol(pstr + 1, &pstr, 10));
		ASSERT(pstr);
		rcPos.bottom = (LONG)(S*_tcstol(pstr + 1, &pstr, 10));
		ASSERT(pstr);
        SIZE szXY = {rcPos.left >= 0 ? rcPos.left : rcPos.right, rcPos.top >= 0 ? rcPos.top : rcPos.bottom};
        SetFixedXY(szXY);
        SetFixedWidth(rcPos.right - rcPos.left);
        SetFixedHeight(rcPos.bottom - rcPos.top);
    }
    else if( _tcsicmp(pstrName, _T("relativepos")) == 0 )
	{
        SIZE szMove,szZoom;
        LPTSTR pstr = NULL;
		szMove.cx = (LONG)(S*_tcstol(pstrValue, &pstr, 10));
		ASSERT(pstr);
		szMove.cy = (LONG)(S*_tcstol(pstr + 1, &pstr, 10));
		ASSERT(pstr);
		szZoom.cx = (LONG)(S*_tcstol(pstr + 1, &pstr, 10));
		ASSERT(pstr);
		szZoom.cy = (LONG)(S*_tcstol(pstr + 1, &pstr, 10));
		ASSERT(pstr);
        SetRelativePos(szMove,szZoom);
    }
    else if( _tcsicmp(pstrName, _T("padding")) == 0 ) {
        RECT rcPadding = { 0 };
        LPTSTR pstr = NULL;
		rcPadding.left = (LONG)(S*_tcstol(pstrValue, &pstr, 10));
		ASSERT(pstr);
		rcPadding.top = (LONG)(S*_tcstol(pstr + 1, &pstr, 10));
		ASSERT(pstr);
		rcPadding.right = (LONG)(S*_tcstol(pstr + 1, &pstr, 10));
		ASSERT(pstr);
		rcPadding.bottom = (LONG)(S*_tcstol(pstr + 1, &pstr, 10));
		ASSERT(pstr);
        SetPadding(rcPadding);
    }
    else if( _tcsicmp(pstrName, _T("bkcolor")) == 0 || _tcsicmp(pstrName, _T("bkcolor1")) == 0 )
	{
        while( *pstrValue > _T('\0') && *pstrValue <= _T(' ') ) pstrValue = ::CharNext(pstrValue);
        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetBkColor(clrColor);
    }
    else if( _tcsicmp(pstrName, _T("bkcolor2")) == 0 ) {
        while( *pstrValue > _T('\0') && *pstrValue <= _T(' ') ) pstrValue = ::CharNext(pstrValue);
        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetBkColor2(clrColor);
    }
    else if( _tcsicmp(pstrName, _T("bkcolor3")) == 0 ) {
        while( *pstrValue > _T('\0') && *pstrValue <= _T(' ') ) pstrValue = ::CharNext(pstrValue);
        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetBkColor3(clrColor);
    }
    else if( _tcsicmp(pstrName, _T("bordercolor")) == 0 ) {
        if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetBorderColor(clrColor);
	}
	else if( _tcsicmp(pstrName, _T("focusbordercolor")) == 0 ) {
		if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
		LPTSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		SetFocusBorderColor(clrColor);
	}
	else if( _tcsicmp(pstrName, _T("disabledbordercolor")) == 0 ) {
		if( *pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
		LPTSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		SetDisabledBorderColor(clrColor);
	}
    else if( _tcsicmp(pstrName, _T("colorhsl")) == 0 ) SetColorHSL(_tcsicmp(pstrValue, _T("true")) == 0);
	else if( _tcsicmp(pstrName, _T("bordersize")) == 0 ) {
		CDuiString nValue = pstrValue;
		if(nValue.Find(',') < 0)
		{
			SetBorderSize(_ttoi(pstrValue));
			RECT rcPadding = {0};
			SetBorderSize(rcPadding);
		}
		else
		{
			RECT rcPadding = { 0 };
			LPTSTR pstr = NULL;
			rcPadding.left = (LONG)(S*_tcstol(pstrValue, &pstr, 10));
			ASSERT(pstr);
			rcPadding.top = (LONG)(S*_tcstol(pstr + 1, &pstr, 10));
			ASSERT(pstr);
			rcPadding.right = (LONG)(S*_tcstol(pstr + 1, &pstr, 10));
			ASSERT(pstr);
			rcPadding.bottom = (LONG)(S*_tcstol(pstr + 1, &pstr, 10));
			ASSERT(pstr);
			SetBorderSize(rcPadding);
		}
	}
	else if (_tcsicmp(pstrName, _T("leftbordersize")) == 0) SetLeftBorderSize((int)(S*_ttoi(pstrValue)));
	else if (_tcsicmp(pstrName, _T("topbordersize")) == 0) SetTopBorderSize((int)(S*_ttoi(pstrValue)));
	else if (_tcsicmp(pstrName, _T("rightbordersize")) == 0) SetRightBorderSize((int)(S*_ttoi(pstrValue)));
	else if (_tcsicmp(pstrName, _T("bottombordersize")) == 0) SetBottomBorderSize((int)(S*_ttoi(pstrValue)));
	else if( _tcsicmp(pstrName, _T("borderstyle")) == 0 ) SetBorderStyle(_ttoi(pstrValue));
    else if( _tcsicmp(pstrName, _T("borderround")) == 0 ) {
        SIZE cxyRound = { 0 };
        LPTSTR pstr = NULL;
        cxyRound.cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
        cxyRound.cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);     
        SetBorderRound(cxyRound);
    }
    else if( _tcsicmp(pstrName, _T("bkimage")) == 0 ) SetBkImage(pstrValue);
	else if (_tcsicmp(pstrName, _T("width")) == 0)
	{
		CDuiString strValue = pstrValue;
		if (strValue.Find('%') <= 0)
		{
			SetFixedWidth((int)(S*_ttoi(pstrValue)));
		}
		else
		{
			m_nWidthScale = _ttoi(pstrValue);
		}
	}
	else if (_tcsicmp(pstrName, _T("height")) == 0)
	{
		CDuiString strValue = pstrValue;
		if (strValue.Find('%') <= 0)
		{
			SetFixedHeight((int)(S*_ttoi(pstrValue)));
		}
		else
		{
			m_nHeightScale = _ttoi(pstrValue);
		}
	}

	else if (_tcsicmp(pstrName, _T("minwidth")) == 0) SetMinWidth((int)(S*_ttoi(pstrValue)));
	else if (_tcsicmp(pstrName, _T("minheight")) == 0) SetMinHeight((int)(S*_ttoi(pstrValue)));
	else if (_tcsicmp(pstrName, _T("maxwidth")) == 0) SetMaxWidth((int)(S*_ttoi(pstrValue)));
	else if (_tcsicmp(pstrName, _T("maxheight")) == 0) SetMaxHeight((int)(S*_ttoi(pstrValue)));
    else if( _tcsicmp(pstrName, _T("name")) == 0 ) SetName(pstrValue);
    else if( _tcsicmp(pstrName, _T("text")) == 0 ) SetText(pstrValue);
    else if( _tcsicmp(pstrName, _T("tooltip")) == 0 ) SetToolTip(pstrValue);
    else if( _tcsicmp(pstrName, _T("userdata")) == 0 ) SetUserData(pstrValue);
    else if( _tcsicmp(pstrName, _T("enabled")) == 0 ) SetEnabled(_tcsicmp(pstrValue, _T("true")) == 0);
    else if( _tcsicmp(pstrName, _T("mouse")) == 0 ) SetMouseEnabled(_tcsicmp(pstrValue, _T("true")) == 0);
	else if( _tcsicmp(pstrName, _T("keyboard")) == 0 ) SetKeyboardEnabled(_tcsicmp(pstrValue, _T("true")) == 0);
    else if( _tcsicmp(pstrName, _T("visible")) == 0 ) SetVisible(_tcsicmp(pstrValue, _T("true")) == 0);
    else if( _tcsicmp(pstrName, _T("float")) == 0 ) SetFloat(_tcsicmp(pstrValue, _T("true")) == 0);
    else if( _tcsicmp(pstrName, _T("shortcut")) == 0 ) SetShortcut(pstrValue[0]);
    else if( _tcsicmp(pstrName, _T("menu")) == 0 ) SetContextMenuUsed(_tcsicmp(pstrValue, _T("true")) == 0);
	else if (_tcsicmp(pstrName, _T("virtualwnd")) == 0) SetVirtualWnd(pstrValue);
	else if (_tcsicmp(pstrName, _T("textcolor")) == 0) {
		if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
		LPTSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		SetTextColor(clrColor);
	}
	else if (_tcsicmp(pstrName, _T("style")) == 0)
	{
		SetStyle(pstrValue);
	}
	else if (_tcsicmp(pstrName, _T("focusdotcolor")) == 0) {
		if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
		LPTSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		SetFocusDotColor(clrColor);
	}
	else if (_tcsicmp(pstrName, _T("focusdotcolor")) == 0) {
		if (*pstrValue == _T('#')) pstrValue = ::CharNext(pstrValue);
		LPTSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		SetFocusDotColor(clrColor);
	}
	else
	{
		//如果不是常规属性，可能是事件
		if (!GetManager())
		{
			return;
		}
		LuaState* L = GetManager()->GetLuaState();
		if (!L || _tcslen(pstrName) < 2 || _tcsnicmp(pstrName, _T("on"), 2) != 0)
		{
			m_attrs.SetAttribute(pstrName, pstrValue);
			return;
		}
		try
		{
			CDuiString strValue(pstrValue);
			CDuiStringArray arValue = strValue.Split('.');
			if (arValue.GetSize() == 1)
			{
				std::string sFun = DuiUtf16ToAscii(strValue);
				BindLuaEvent(pstrName + 2, L->getGlobal(sFun.c_str()));
			}
			else
			{
				std::string sModule = DuiUtf16ToAscii(arValue[0]);
				LuaTable tab=L->require(sModule.c_str());
				if (tab.isValid())
				{
					std::string sFun = DuiUtf16ToAscii(arValue[1]);
					BindLuaEvent(pstrName + 2,tab.getTable(sFun.c_str()));
				}
			}
		}
		catch(LuaException err)
		{
			OutputDebugStringA(err.what());
			//LOGE("doString error:"<<err.what());
		}
	}
}

void CControlUI::ApplyAttributeList(LPCTSTR pstrList)
{
    CDuiString sItem;
    CDuiString sValue;
	if (!pstrList)
	{
		return;
	}
    while( *pstrList != _T('\0') ) {
		
		std::vector<TCHAR> sep;

        sItem.Empty();
        sValue.Empty();
        while( *pstrList != _T('\0') && *pstrList != _T('=') ) {
            LPTSTR pstrTemp = ::CharNext(pstrList);
            while( pstrList < pstrTemp) {
                sItem += *pstrList++;
            }
        }

        ASSERT( *pstrList == _T('=') );
        if( *pstrList++ != _T('=') ) return;
		ASSERT(*pstrList == _T('\"') || *pstrList == _T('\''));		
		if (*pstrList != _T('\"') && *pstrList != _T('\'')) return;
		sep.push_back(*pstrList);
		pstrList++;
		
        //while( *pstrList != _T('\0') && *pstrList != _T('\"') ) {
		while (*pstrList != _T('\0') && sep.size() != 0) 
		{
            LPTSTR pstrTemp = ::CharNext(pstrList);
            while( pstrList < pstrTemp) {
                sValue += *pstrList++;
            }

			TCHAR s = sep.back();
			if (s == *pstrList)
			{
				sep.pop_back();
			}
			else
			{
				s = *pstrList;
				if (s == _T('\"') || s == _T('\''))
				{
					sep.push_back(s);
				}
			}
        }
        //ASSERT( *pstrList == _T('\"') );
        //if( *pstrList++ != _T('\"') ) return this;
		pstrList++;
        SetAttribute(sItem, sValue);
        if( *pstrList++ != _T(' ') )
			return;
    }
}

SIZE CControlUI::EstimateSize(SIZE szAvailable)
{
	SIZE si = {GetFixedWidth(), GetFixedHeight()};
	if (m_nWidthScale == 0 &&
		m_nHeightScale == 0)
	{
		return si;
	}

	CContainerUI * pParentUI = (CContainerUI *)GetParent();
	if (!pParentUI)
	{
		return si;
	}

	RECT rcParent = pParentUI->GetPos();
	RECT rcParentInset = pParentUI->GetInset();
	int nPaddingX = rcParentInset.left + rcParentInset.right;
	int nPaddingY = rcParentInset.top + rcParentInset.bottom;

	for (int it = 0; it < pParentUI->GetItems().GetSize(); it++)
	{
		CControlUI* pControl = static_cast<CControlUI*>(pParentUI->GetItems()[it]);
		if (!pControl)
		{
			continue;
		}

		RECT rcPadding = pControl->GetPadding();

		nPaddingX += rcPadding.left + rcPadding.right;
		nPaddingY += rcPadding.top + rcPadding.bottom;

	}

	if (m_nWidthScale != 0)
	{
		si.cx = m_nWidthScale * (rcParent.right - rcParent.left - nPaddingX) / 100;
	}

	if (m_nHeightScale != 0)
	{
		si.cy = m_nHeightScale * (rcParent.bottom - rcParent.top - nPaddingY) / 100;
	}

	return si;
}

void CControlUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
    if( !::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem) ) return;

    // 绘制循序：背景颜色->背景图->状态图->文本->边框
    if( m_cxyBorderRound.cx > 0 || m_cxyBorderRound.cy > 0 ) {
        CRenderClip roundClip;
        CRenderClip::GenerateRoundClip(hDC, m_rcPaint,  m_rcItem, m_cxyBorderRound.cx, m_cxyBorderRound.cy, roundClip);
        PaintBkColor(hDC);
        PaintBkImage(hDC);
		PaintStatusImage(hDC);
		PaintForeColor(hDC);
		PaintForeImage(hDC);
        PaintText(hDC);
        PaintBorder(hDC);
    }
    else
	{
        PaintBkColor(hDC);
        PaintBkImage(hDC);
		PaintStatusImage(hDC);
		PaintForeColor(hDC);
		PaintForeImage(hDC);
        PaintText(hDC);
        PaintBorder(hDC);
    }
	PaintFocusedDot(hDC);
}

void CControlUI::PaintFocusedDot(HDC hDC)
{
	if (IsFocused())
	{
		if (m_pManager->GetShowFocusDot() && m_pManager->GetNeedShowFocusDot())
		{
			CDuiRect rc(m_rcItem);
			rc.Deflate(1, 1);
			DWORD dwFocusDotColor = GetFocusDotColor();
			if (!dwFocusDotColor)
			{
				dwFocusDotColor = m_pManager->GetDefaultFocusDotColor();
			}
			CRenderEngine::DrawRect(hDC, rc, 1, dwFocusDotColor, PS_DOT);
		}
	}
}

void CControlUI::PaintBkColor(HDC hDC)
{
	DWORD dwBkColor = GetStatusColor(DUI_ATTR_POS_BK DUI_ATTR_COLOR);
	DWORD dwBackColor2 = GetStatusColor(DUI_ATTR_POS_BK DUI_ATTR_COLOR2);
	DWORD dwBackColor3 = GetStatusColor(DUI_ATTR_POS_BK DUI_ATTR_COLOR3);
	
    if( dwBkColor != 0 ) {
        if( dwBackColor2 != 0 ) {
            if( dwBackColor3 != 0 ) {
                RECT rc = m_rcItem;
                rc.bottom = (rc.bottom + rc.top) / 2;
                CRenderEngine::DrawGradient(hDC, rc, GetAdjustColor(dwBkColor), GetAdjustColor(dwBackColor2), true, 8);
                rc.top = rc.bottom;
                rc.bottom = m_rcItem.bottom;
                CRenderEngine::DrawGradient(hDC, rc, GetAdjustColor(dwBackColor2), GetAdjustColor(dwBackColor3), true, 8);
            }
            else 
                CRenderEngine::DrawGradient(hDC, m_rcItem, GetAdjustColor(dwBkColor), GetAdjustColor(dwBackColor2), true, 16);
        }
        else if( dwBkColor >= 0xFF000000 ) CRenderEngine::DrawColor(hDC, m_rcPaint, GetAdjustColor(dwBkColor));
        else CRenderEngine::DrawColor(hDC, m_rcItem, GetAdjustColor(dwBkColor));
    }
}

void CControlUI::PaintBkImage(HDC hDC)
{
	const CDuiImage img = GetStatusImage(DUI_ATTR_POS_BK DUI_ATTR_IMAGE);
	//const CDuiImage img = GetBkImage();
    if(!img) return;

	if (m_gifBk.IsValid())
	{
		m_gifBk.DrawFrame( hDC, m_rcItem );
		return;
	}

    if( !DrawImage(hDC, img) )
	{
		//m_attrs.SetAttribute(DUI_ATTR_POS_BK DUI_ATTR_IMAGE, _T(""));
		//m_sBkImage.Empty();
	}
}

void CControlUI::PaintStatusImage(HDC hDC)
{
    return;
}

void CControlUI::PaintForeColor(HDC hDC)
{
	DWORD dwBkColor = GetStatusColor(DUI_ATTR_POS_FORE DUI_ATTR_COLOR);
	DWORD dwBackColor2 = GetStatusColor(DUI_ATTR_POS_FORE DUI_ATTR_COLOR2);
	DWORD dwBackColor3 = GetStatusColor(DUI_ATTR_POS_FORE DUI_ATTR_COLOR3);

	if( dwBkColor != 0 ) {
		if( dwBackColor2 != 0 ) {
			if( dwBackColor3 != 0 ) {
				RECT rc = m_rcItem;
				rc.bottom = (rc.bottom + rc.top) / 2;
				CRenderEngine::DrawGradient(hDC, rc, GetAdjustColor(dwBkColor), GetAdjustColor(dwBackColor2), true, 8);
				rc.top = rc.bottom;
				rc.bottom = m_rcItem.bottom;
				CRenderEngine::DrawGradient(hDC, rc, GetAdjustColor(dwBackColor2), GetAdjustColor(dwBackColor3), true, 8);
			}
			else 
				CRenderEngine::DrawGradient(hDC, m_rcItem, GetAdjustColor(dwBkColor), GetAdjustColor(dwBackColor2), true, 16);
		}
		else if( dwBkColor >= 0xFF000000 ) CRenderEngine::DrawColor(hDC, m_rcPaint, GetAdjustColor(dwBkColor));
		else CRenderEngine::DrawColor(hDC, m_rcItem, GetAdjustColor(dwBkColor));
	}
}

void CControlUI::PaintForeImage(HDC hDC)
{
	const CDuiImage img = GetStatusImage(DUI_ATTR_POS_FORE DUI_ATTR_IMAGE);
	if(!img) return;

	if( !DrawImage(hDC, img) )
	{
	}
}

void CControlUI::PaintText(HDC hDC)
{
    return;
}

void CControlUI::PaintBorder(HDC hDC)
{
	DWORD dwBorderColor = GetStatusColor(DUI_ATTR_POS_BORDER DUI_ATTR_COLOR);
// 	DWORD dwBackColor2 = GetStatusColor(DUI_ATTR_POS_FORE DUI_ATTR_COLOR2);
// 	DWORD dwBackColor3 = GetStatusColor(DUI_ATTR_POS_FORE DUI_ATTR_COLOR3);
//	DWORD dwBorderColor = GetBorderColor();
// 	DWORD dwFocusBorderColor = GetFocusBorderColor();
// 	DWORD dwDisabledBorderColor = GetDisabledBorderColor();
	if(dwBorderColor != 0)
	{
		if(m_nBorderSize > 0 && ( m_cxyBorderRound.cx > 0 || m_cxyBorderRound.cy > 0 ))//画圆角边框
		{
			CRenderEngine::DrawRoundRect(hDC, m_rcItem, m_nBorderSize, m_cxyBorderRound.cx, m_cxyBorderRound.cy, GetAdjustColor(dwBorderColor));
		}
		else
		{
			if(m_rcBorderSize.left > 0 || m_rcBorderSize.top > 0 || m_rcBorderSize.right > 0 || m_rcBorderSize.bottom > 0)
			{
				RECT rcBorder;
				if(m_rcBorderSize.left > 0){
					rcBorder		= m_rcItem;
					rcBorder.right	= m_rcItem.left;
					CRenderEngine::DrawLine(hDC,rcBorder,m_rcBorderSize.left,GetAdjustColor(dwBorderColor),m_nBorderStyle);
				}
				if(m_rcBorderSize.top > 0){
					rcBorder		= m_rcItem;
					rcBorder.bottom	= m_rcItem.top;
					CRenderEngine::DrawLine(hDC,rcBorder,m_rcBorderSize.top,GetAdjustColor(dwBorderColor),m_nBorderStyle);
				}
				if(m_rcBorderSize.right > 0){
					rcBorder		= m_rcItem;
					rcBorder.left	= m_rcItem.right;
					CRenderEngine::DrawLine(hDC,rcBorder,m_rcBorderSize.right,GetAdjustColor(dwBorderColor),m_nBorderStyle);
				}
				if(m_rcBorderSize.bottom > 0){
					rcBorder		= m_rcItem;
					rcBorder.top	= m_rcItem.bottom;
					CRenderEngine::DrawLine(hDC,rcBorder,m_rcBorderSize.bottom,GetAdjustColor(dwBorderColor),m_nBorderStyle);
				}
			}
			else if(m_nBorderSize > 0)
				CRenderEngine::DrawRect(hDC, m_rcItem, m_nBorderSize, GetAdjustColor(dwBorderColor), m_nBorderStyle);
		}
	}
}

void CControlUI::DoPostPaint(HDC hDC, const RECT& rcPaint)
{
    return;
}

int CControlUI::GetLeftBorderSize() const
{
	return m_rcBorderSize.left;
}

void CControlUI::SetLeftBorderSize( int nSize )
{
	m_rcBorderSize.left = nSize;
	Invalidate();
}

int CControlUI::GetTopBorderSize() const
{
	return m_rcBorderSize.top;
}

void CControlUI::SetTopBorderSize( int nSize )
{
	m_rcBorderSize.top = nSize;
	Invalidate();
}

int CControlUI::GetRightBorderSize() const
{
	return m_rcBorderSize.right;
}

void CControlUI::SetRightBorderSize( int nSize )
{
	m_rcBorderSize.right = nSize;
	Invalidate();
}

int CControlUI::GetBottomBorderSize() const
{
	return m_rcBorderSize.bottom;
}

void CControlUI::SetBottomBorderSize( int nSize )
{
	m_rcBorderSize.bottom = nSize;
	Invalidate();
}

int CControlUI::GetBorderStyle() const
{
	return m_nBorderStyle;
}

void CControlUI::SetBorderStyle( int nStyle )
{
	m_nBorderStyle = nStyle;
	Invalidate();
}

void CControlUI::OnTimer( UINT_PTR idEvent )
{
	m_gifBk.OnTimer(idEvent);
}

CDuiString CControlUI::GetStyle()
{
	return m_strStyle;
}

void CControlUI::SetStyle(LPCTSTR lpszStyle)
{
	m_strStyle = lpszStyle;
	CDuiString strStyle;
	for (int i = 0; lpszStyle[i]; ++i)
	{
		if (lpszStyle[i] == ' ' || lpszStyle[i] == '\t' || lpszStyle[i] == ',' || lpszStyle[i] == ';')
		{
			if (strStyle.GetLength())
			{
				LPCTSTR lpszAttrs = m_pManager->GetDefaultAttributeList(strStyle);
				ApplyAttributeList(lpszAttrs);
				strStyle.Empty();
			}
			continue;
		}
		strStyle += lpszStyle[i];
	}
	if (strStyle.GetLength())
	{
		LPCTSTR lpszAttrs = m_pManager->GetDefaultAttributeList(strStyle);
		ApplyAttributeList(lpszAttrs);
		strStyle.Empty();
	}
}

DWORD CControlUI::GetStatus()
{
	DWORD dwStatus = 0;
	if (!IsEnabled())
	{
		dwStatus |= UISTATE_DISABLED;
	}
	if (IsFocused())
	{
		dwStatus |= UISTATE_FOCUSED;
	}
	return dwStatus;
}

DuiLib::CDuiString CControlUI::GetStatusString( DWORD dwStatus /*= 0*/, int nIgnorStatus /*= 0*/)
{
	CDuiString sStatus;
	if (dwStatus == 0)
	{
		dwStatus = GetStatus();
	}
	if (dwStatus & UISTATE_DISABLED && nIgnorStatus-- <= 0)
	{
		sStatus += DUI_ATTR_STATUS_DISABLED;
	}
	if (dwStatus & UISTATE_FOCUSED && nIgnorStatus-- <= 0)
	{
		sStatus += DUI_ATTR_STATUS_FOCUSED;
	}
	if (dwStatus & UISTATE_PUSHED && nIgnorStatus-- <= 0)
	{
		sStatus += DUI_ATTR_STATUS_PUSHED;
	}
	if (dwStatus & UISTATE_HOT && nIgnorStatus-- <= 0)
	{
		sStatus += DUI_ATTR_STATUS_HOT;
	}
	if (dwStatus & UISTATE_SELECTED && nIgnorStatus-- <= 0)
	{
		sStatus += DUI_ATTR_STATUS_SELECTED;
	}
	return sStatus;
}

DWORD CControlUI::GetStatusColor(LPCTSTR lpszAttr)
{
	CDuiString sStatus = GetStatusString(GetStatus());
	int nIgnorStatus = 0;
	while (sStatus.GetLength())
	{
		DWORD dwColor = m_attrs.GetColor(sStatus + lpszAttr);
		if (dwColor)
		{
			return dwColor;
		}
		sStatus = GetStatusString(GetStatus(), ++nIgnorStatus);
	}
	return m_attrs.GetColor(lpszAttr);
}

DuiLib::CDuiImage CControlUI::GetStatusImage(LPCTSTR lpszAttr)
{
	CDuiString sStatus = GetStatusString(GetStatus());
	int nIgnorStatus = 0;
	while (sStatus.GetLength())
	{
		CDuiImage img = m_attrs.GetImage(sStatus + lpszAttr);
		if (img)
		{
			return img;
		}
		sStatus = GetStatusString(GetStatus(), ++nIgnorStatus);
	}
	return m_attrs.GetImage(lpszAttr);
}

} // namespace DuiLib
