#ifndef __UIMANAGER_H__
#define __UIMANAGER_H__

#pragma once

#include <vector>
#include <map>
#include "../lua/base/RefBase.h"
#include "../lua/base/Criticalsection.h"

namespace DuiLib {

class CControlUI;
class CRichEditUI;
class CDuiTrayIconUI;
enum EVENTTYPE_UI
{
    UIEVENT__FIRST = 1,
    UIEVENT__KEYBEGIN,
    UIEVENT_KEYDOWN,
    UIEVENT_KEYUP,
    UIEVENT_CHAR,
    UIEVENT_SYSKEY,
    UIEVENT__KEYEND,
    UIEVENT__MOUSEBEGIN,
    UIEVENT_MOUSEMOVE,
    UIEVENT_MOUSELEAVE,
    UIEVENT_MOUSEENTER,
    UIEVENT_MOUSEHOVER,
    UIEVENT_BUTTONDOWN,
    UIEVENT_BUTTONUP,
    UIEVENT_RBUTTONDOWN,
	UIEVENT_DBLCLICK,
    UIEVENT_CONTEXTMENU,
    UIEVENT_SCROLLWHEEL,
    UIEVENT__MOUSEEND,
    UIEVENT_KILLFOCUS,
    UIEVENT_SETFOCUS,
    UIEVENT_WINDOWSIZE,
    UIEVENT_SETCURSOR,
    UIEVENT_TIMER,
    UIEVENT_NOTIFY,
	UIEVENT_COMMAND,
	UIEVENT_TRAYICON,
	UIEVENT_GESTURE,
    UIEVENT__LAST,
};


// Flags for CControlUI::GetControlFlags()
#define UIFLAG_TABSTOP       0x00000001
#define UIFLAG_SETCURSOR     0x00000002
#define UIFLAG_WANTRETURN    0x00000004

// Flags for FindControl()
#define UIFIND_ALL           0x00000000
#define UIFIND_VISIBLE       0x00000001
#define UIFIND_ENABLED       0x00000002
#define UIFIND_HITTEST       0x00000004
#define UIFIND_TOP_FIRST     0x00000008
#define UIFIND_ME_FIRST      0x80000000

// Flags for the CDialogLayout stretching
#define UISTRETCH_NEWGROUP   0x00000001
#define UISTRETCH_NEWLINE    0x00000002
#define UISTRETCH_MOVE_X     0x00000004
#define UISTRETCH_MOVE_Y     0x00000008
#define UISTRETCH_SIZE_X     0x00000010
#define UISTRETCH_SIZE_Y     0x00000020

// Flags used for controlling the paint
#define UISTATE_FOCUSED      0x00000001
#define UISTATE_SELECTED     0x00000002
#define UISTATE_DISABLED     0x00000004
#define UISTATE_HOT          0x00000008
#define UISTATE_PUSHED       0x00000010
#define UISTATE_READONLY     0x00000020
#define UISTATE_CAPTURED     0x00000040



typedef struct tagTFontInfo
{
    HFONT hFont;
    CDuiString sFontName;
    int iSize;
    bool bBold;
    bool bUnderline;
    bool bItalic;
    TEXTMETRIC tm;
} TFontInfo;

typedef struct tagTImageInfo
{
    HBITMAP hBitmap;
    int nX;
    int nY;
    bool alphaChannel;
    CDuiString sResType;
    DWORD dwMask;
} TImageInfo;

// Structure for notifications from the system
// to the control implementation.
typedef struct tagTEventUI
{
    int Type;
    CControlUI* pSender;
    DWORD dwTimestamp;
    POINT ptMouse;
    TCHAR chKey;
    WORD wKeyState;
    WPARAM wParam;
    LPARAM lParam;
} TEventUI;

// Structure for relative position to the parent
typedef struct tagTRelativePosUI
{
	bool bRelative;
	SIZE szParent;
	int nMoveXPercent;
	int nMoveYPercent;
	int nZoomXPercent;
	int nZoomYPercent;
}TRelativePosUI;

// Listener interface
class INotifyUI
{
public:
    virtual void Notify(TNotifyUI& msg) = 0;
};

// MessageFilter interface
class IMessageFilterUI
{
public:
    virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled) = 0;
};

class ITranslateAccelerator
{
public:
	virtual LRESULT TranslateAccelerator(MSG *pMsg) = 0;
};

class IRunbaleUI:public base::RefCountedBase
{
public:
	virtual void Run(LuaState* L)=0;
};

typedef CControlUI* (*LPCREATECONTROL)(LPCTSTR pstrType);


class UILIB_API CPaintManagerUI
{
public:
	LBIND_BASE_CLASS_DEFINE(CPaintManagerUI);
    CPaintManagerUI();
    ~CPaintManagerUI();

public:
    void Init(HWND hWnd);
    void NeedUpdate();
    void Invalidate(RECT& rcItem);

    HDC GetPaintDC() const;
    HWND GetPaintWindow() const;
    HWND GetTooltipWindow() const;

	double GetDpiScale() const;

    POINT GetMousePos() const;
    SIZE GetClientSize() const;
    SIZE GetInitSize();
    void SetInitSize(int cx, int cy);
    RECT& GetSizeBox();
    void SetSizeBox(RECT& rcSizeBox);
    RECT& GetCaptionRect();
    void SetCaptionRect(RECT& rcCaption);
    SIZE GetRoundCorner() const;
    void SetRoundCorner(int cx, int cy);
    SIZE GetMinInfo() const;
    void SetMinInfo(int cx, int cy);
    SIZE GetMaxInfo() const;
    void SetMaxInfo(int cx, int cy);
	int GetTransparent() const;
    void SetTransparent(int nOpacity);
    void SetBackgroundTransparent(bool bTrans);
    bool IsShowUpdateRect() const;
    void SetShowUpdateRect(bool show);
	//redrain
	bool IsBackgroundTransparent();
	bool ShowCaret(bool bShow);
	bool SetCaretPos(CRichEditUI* obj, int x, int y);
	CRichEditUI* GetCurrentCaretObject();
	bool CreateCaret(HBITMAP hBmp, int nWidth, int nHeight);
	void DrawCaret(HDC hDC, const RECT& rcPaint);
	CShadowUI* GetShadow();
	void SetUseGdiplusText(bool bUse);
	bool IsUseGdiplusText() const;

    static HINSTANCE GetInstance();
    static CDuiString GetInstancePath();
    static CDuiString GetCurrentPath();
    static HINSTANCE GetResourceDll();
    static const CDuiString& GetResourcePath();
    static const CDuiString& GetResourceZip();
    static bool IsCachedResourceZip();
    static HANDLE GetResourceZipHandle();
    static void SetInstance(HINSTANCE hInst);
    static void SetCurrentPath(LPCTSTR pStrPath);
    static void SetResourceDll(HINSTANCE hInst);
    static void SetResourcePath(LPCTSTR pStrPath);
	static void SetResourceZip(LPVOID pVoid, unsigned int len);
    static void SetResourceZip(LPCTSTR pstrZip, bool bCachedResourceZip = false);
    static void GetHSL(short* H, short* S, short* L);
    static void SetHSL(bool bUseHSL, short H, short S, short L); // H:0~360, S:0~200, L:0~200 
    static void ReloadSkin();
    static bool LoadPlugin(LPCTSTR pstrModuleName);
    static CStdPtrArray* GetPlugins();

    bool UseParentResource(CPaintManagerUI* pm);
    CPaintManagerUI* GetParentResource() const;

    DWORD GetDefaultDisabledColor() const;
    void SetDefaultDisabledColor(DWORD dwColor);
    DWORD GetDefaultFontColor() const;
    void SetDefaultFontColor(DWORD dwColor);
    DWORD GetDefaultLinkFontColor() const;
    void SetDefaultLinkFontColor(DWORD dwColor);
    DWORD GetDefaultLinkHoverFontColor() const;
    void SetDefaultLinkHoverFontColor(DWORD dwColor);
    DWORD GetDefaultSelectedBkColor() const;
	void SetDefaultSelectedBkColor(DWORD dwColor);
	DWORD GetDefaultFocusDotColor() const;
	void SetDefaultFocusDotColor(DWORD dwColor);
	DWORD GetShowFocusDot() const;
	void SetShowFocusDot(bool bShow);
	DWORD GetNeedShowFocusDot() const;
	void SetNeedShowFocusDot(bool bShow);
    TFontInfo* GetDefaultFontInfo();
    void SetDefaultFont(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);
    DWORD GetCustomFontCount() const;
    HFONT AddFont(LPCTSTR pStrFontID, LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic, bool bCheckExist = false);
    HFONT AddFontAt(int index, LPCTSTR pStrFontID, LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);
    HFONT GetFont(LPCTSTR lpszFontID);
    HFONT GetFont(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);
    bool FindFont(HFONT hFont);
    bool FindFont(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);
    int GetFontIndex(HFONT hFont);
    int GetFontIndex(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);
    bool RemoveFont(HFONT hFont);
    void RemoveAllFonts();
    TFontInfo* GetFontInfo(LPCTSTR lpszFontID);
    TFontInfo* GetFontInfo(HFONT hFont);

    const TImageInfo* GetImage(LPCTSTR bitmap);
    const TImageInfo* GetImageEx(LPCTSTR bitmap, LPCTSTR type = NULL, DWORD mask = 0);
    const TImageInfo* AddImage(LPCTSTR bitmap, LPCTSTR type = NULL, DWORD mask = 0);
    const TImageInfo* AddImage(LPCTSTR bitmap, HBITMAP hBitmap, int iWidth, int iHeight, bool bAlpha);
    bool RemoveImage(LPCTSTR bitmap);
    void RemoveAllImages();
    void ReloadAllImages();

    void AddDefaultAttributeList(LPCTSTR pStrControlName, LPCTSTR pStrControlAttrList);
    LPCTSTR GetDefaultAttributeList(LPCTSTR pStrControlName) const;
    bool RemoveDefaultAttributeList(LPCTSTR pStrControlName);
    const CStdStringPtrMap& GetDefaultAttribultes() const;
    void RemoveAllDefaultAttributeList();

	void AddMacro(LPCTSTR pstrName, LPCTSTR pstrValue);
	LPCTSTR GetMacro(LPCTSTR pstrName) const;
	bool RemoveMacro(LPCTSTR pstrName);
	void RemoveAllMacro();
	CDuiString FormatByMacro(LPCTSTR lpszValue);

    bool AttachDialog(CControlUI* pControl);
    bool InitControls(CControlUI* pControl, CControlUI* pParent = NULL);
    void ReapObjects(CControlUI* pControl);

    bool AddOptionGroup(LPCTSTR pStrGroupName, CControlUI* pControl);
    CStdPtrArray* GetOptionGroup(LPCTSTR pStrGroupName);
    void RemoveOptionGroup(LPCTSTR pStrGroupName, CControlUI* pControl);
    void RemoveAllOptionGroups();

    CControlUI* GetFocus() const;
    void SetFocus(CControlUI* pControl);
    void SetFocusNeeded(CControlUI* pControl);

    bool SetNextTabControl(bool bForward = true);

    bool SetTimer(CControlUI* pControl, UINT nTimerID, UINT uElapse);
    bool KillTimer(CControlUI* pControl, UINT nTimerID);
    void KillTimer(CControlUI* pControl);
    void RemoveAllTimers();

    void SetCapture();
    void ReleaseCapture();
    bool IsCaptured();

    bool AddNotifier(INotifyUI* pControl);
    bool RemoveNotifier(INotifyUI* pControl);   
    void SendNotify(TNotifyUI& Msg, bool bAsync = false);
    void SendNotify(CControlUI* pControl, LPCTSTR pstrMessage, WPARAM wParam = 0, LPARAM lParam = 0, bool bAsync = false);

    bool AddPreMessageFilter(IMessageFilterUI* pFilter);
    bool RemovePreMessageFilter(IMessageFilterUI* pFilter);

    bool AddMessageFilter(IMessageFilterUI* pFilter);
    bool RemoveMessageFilter(IMessageFilterUI* pFilter);

    int GetPostPaintCount() const;
    bool AddPostPaint(CControlUI* pControl);
    bool RemovePostPaint(CControlUI* pControl);
    bool SetPostPaintIndex(CControlUI* pControl, int iIndex);

    void AddDelayedCleanup(CControlUI* pControl);

	bool AddTranslateAccelerator(ITranslateAccelerator *pTranslateAccelerator);
	bool RemoveTranslateAccelerator(ITranslateAccelerator *pTranslateAccelerator);
	bool TranslateAccelerator(LPMSG pMsg);

    CControlUI* GetRoot() const;
    CControlUI* FindControl(POINT pt) const;
    CControlUI* FindControl(LPCTSTR pstrName) const;
    CControlUI* FindSubControlByPoint(CControlUI* pParent, POINT pt) const;
    CControlUI* FindSubControlByName(CControlUI* pParent, LPCTSTR pstrName) const;
    CControlUI* FindSubControlByClass(CControlUI* pParent, LPCTSTR pstrClass, int iIndex = 0);
    CStdPtrArray* FindSubControlsByClass(CControlUI* pParent, LPCTSTR pstrClass);
    CStdPtrArray* GetSubControlsByClass();

	int			FindControl(LPCTSTR pstrName, CStdPtrArray& ctrls) const;
	void		RemoveControl(LPCTSTR pstrName, CControlUI* pControl);
	void		SetControlVisible(LPCTSTR pstrName, bool bVisible);
	void		EnalbeControl(LPCTSTR pstrName, bool bEnable);
	void		SelectControl(LPCTSTR pstrName, bool bSelect);

    static void MessageLoop();
    static bool TranslateMessage(const LPMSG pMsg);
	static void Term();

    bool MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes);

	void OnClose();

	bool PreMessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes);
	void UsedVirtualWnd(bool bUsed);

	CDuiTrayIconUI* GetTrayObject(){return m_pDuiTray;}
	void SetWindowTitile(LPCTSTR lpTitle);
	CDuiStringTable& GetStringTable(){return m_StringTable;}

	LuaState* GetLuaState();
	bool CheckAvalible();
	RefCountedPtr<IRunbaleUI> GetRunable();
	//获取Control绑定的事件表，如果存在直接返回，否则判断bCreate决定是否创建
	LuaObject GetControlEventMap(CControlUI* ctl,bool bCreate);

private:
    static CControlUI* CALLBACK __FindControlFromNameHash(CControlUI* pThis, LPVOID pData);
    static CControlUI* CALLBACK __FindControlFromCount(CControlUI* pThis, LPVOID pData);
    static CControlUI* CALLBACK __FindControlFromPoint(CControlUI* pThis, LPVOID pData);
    static CControlUI* CALLBACK __FindControlFromTab(CControlUI* pThis, LPVOID pData);
    static CControlUI* CALLBACK __FindControlFromShortcut(CControlUI* pThis, LPVOID pData);
    static CControlUI* CALLBACK __FindControlFromUpdate(CControlUI* pThis, LPVOID pData);
    static CControlUI* CALLBACK __FindControlFromName(CControlUI* pThis, LPVOID pData);
    static CControlUI* CALLBACK __FindControlFromClass(CControlUI* pThis, LPVOID pData);
    static CControlUI* CALLBACK __FindControlsFromClass(CControlUI* pThis, LPVOID pData);

	bool OnSysCommand(WPARAM wParam, LPARAM lParam);
	bool OnGetMinMaxInfo(WPARAM wParam, LPARAM lParam);
	bool OnSize(WPARAM wParam, LPARAM lParam);
	bool OnTimer(WPARAM wParam, LPARAM lParam);
	bool OnMouseHover(WPARAM wParam, LPARAM lParam);
	bool OnMouseLeave(WPARAM wParam, LPARAM lParam);
	bool OnMouseMove(WPARAM wParam, LPARAM lParam);
	bool OnLButtonDown(WPARAM wParam, LPARAM lParam);
	bool OnLButtonDBClick(WPARAM wParam, LPARAM lParam);
	bool OnLButtonUp(WPARAM wParam, LPARAM lParam);
	bool OnRButtonDown(WPARAM wParam, LPARAM lParam);
	bool OnContextMenu(WPARAM wParam, LPARAM lParam);
	bool OnMouseWheel(WPARAM wParam, LPARAM lParam);
	bool OnChar(WPARAM wParam, LPARAM lParam);
	bool OnKeyDown(WPARAM wParam, LPARAM lParam);
	bool OnKeyUp(WPARAM wParam, LPARAM lParam);
	bool OnSetCursor(WPARAM wParam, LPARAM lParam);
	bool OnGesture(WPARAM wParam, LPARAM lParam);
private:
	double	m_nScale;

    HWND m_hWndPaint;
	int m_nOpacity;
    HDC m_hDcPaint;
	LPBYTE m_pBmpBackgroundBits;
    HDC m_hDcOffscreen;
    HDC m_hDcBackground;
    HBITMAP m_hbmpOffscreen;
    HBITMAP m_hbmpBackground;
    HWND m_hwndTooltip;

	/*
	typedef struct tagTOOLINFOA {
		UINT cbSize;
		UINT uFlags;
		HWND hwnd;
		UINT_PTR uId;
		RECT rect;
		HINSTANCE hinst;
		LPSTR lpszText;
		#if (_WIN32_IE >= 0x0300)
		LPARAM lParam;
		#endif
		#if (_WIN32_WINNT >= 0x0501)
		void *lpReserved;
		#endif
	} TTTOOLINFOA, NEAR *PTOOLINFOA, *LPTTTOOLINFOA;

	TOOLINFO在不同的版本中的定义不同，如果库中定义的_WIN32_WINNT小于0x0501，
	应用程序中定义的大于等于0x0501，就会出现错误,
	因此采用共用体占位，共用体的大小不要小于sizeof(TTTOOLINFO)就行了
	*/
	union
    {
		TOOLINFO m_ToolTip;
		BYTE byReserved[64];
	};
    bool m_bShowUpdateRect;
	bool m_bIsRestore;
	//redrain
	RECT m_rtCaret;
	bool m_bCaretActive;
	bool m_bCaretShowing;
	CRichEditUI* m_currentCaretObject;

	CShadowUI m_shadow;
	bool m_bUseGdiplusText;
    //
	//
	ULONG_PTR						m_gdiplusToken;
	Gdiplus::GdiplusStartupInput	*m_pGdiplusStartupInput;
	//
    CControlUI* m_pRoot;
    CControlUI* m_pFocus;
    CControlUI* m_pEventHover;
    CControlUI* m_pEventClick;
    CControlUI* m_pEventKey;
    //
    POINT m_ptLastMousePos;
    SIZE m_szMinWindow;
    SIZE m_szMaxWindow;
    SIZE m_szInitWindowSize;
    RECT m_rcSizeBox;
    SIZE m_szRoundCorner;
    RECT m_rcCaption;
    UINT m_uTimerID;
    bool m_bUpdateNeeded;
    bool m_bFocusNeeded;
    bool m_bOffscreenPaint;
    bool m_bAlphaBackground;
    bool m_bMouseTracking;
    bool m_bMouseCapture;
	bool m_bUsedVirtualWnd;

    //
    CStdPtrArray m_aNotifiers;
    CStdPtrArray m_aTimers;
    CStdPtrArray m_aPreMessageFilters;
    CStdPtrArray m_aMessageFilters;
    CStdPtrArray m_aPostPaintControls;
    CStdPtrArray m_aDelayedCleanup;
    CStdPtrArray m_aAsyncNotify;
    CStdPtrArray m_aFoundControls;
	/*
	struct lessDuiString
	{	// functor for operator<
		typedef CDuiString first_argument_type;
		typedef CDuiString second_argument_type;
		typedef bool result_type;

		constexpr bool operator()(const CDuiString& _Left, const CDuiString& _Right) const
		{	// apply operator< to operands
			return _Left < _Right;
		}
	};
	*/
	typedef std::multimap<CDuiString, LPVOID> CMulMapStrToPtr;
    //CStdStringPtrMap m_mNameHash;
	CMulMapStrToPtr*	m_pMapNameToCtrl;
    CStdStringPtrMap m_mOptionGroup;
    //
    CPaintManagerUI* m_pParentResourcePM;
    DWORD m_dwDefaultDisabledColor;
    DWORD m_dwDefaultFontColor;
    DWORD m_dwDefaultLinkFontColor;
    DWORD m_dwDefaultLinkHoverFontColor;
	DWORD m_dwDefaultSelectedBkColor;
	DWORD m_dwDefaultFocusDotColor;
	bool m_bShowFocusDot;
	bool m_bNeedShowFocusDot;
    TFontInfo m_DefaultFontInfo;
	CStdStringPtrMap m_mapFont;
    //CStdPtrArray m_aCustomFonts;

	CStdStringPtrMap m_mImageHash;
	CStdStringPtrMap m_DefaultAttrHash;
	CStdStringPtrMap m_MacroHash;
    //
    static HINSTANCE m_hInstance;
    static HINSTANCE m_hResourceInstance;
    static CDuiString m_pStrResourcePath;
    static CDuiString m_pStrResourceZip;
    static bool m_bCachedResourceZip;
    static HANDLE m_hResourceZip;
	static CDuiBuffer m_BufferResourceZip;
    static short m_H;
    static short m_S;
    static short m_L;
    static CStdPtrArray m_aPreMessages;
    static CStdPtrArray m_aPlugins;

	CDuiTrayIconUI* m_pDuiTray;
	CDuiString	m_sTitile;

	LuaEngine m_lua;
	DWORD m_threadId;
	std::queue<RefCountedPtr<IRunbaleUI>> m_runableQueue;
	base::CriticalSection m_queueLock;
public:
	CDuiString m_pStrDefaultFontName;
	CStdPtrArray m_aTranslateAccelerator;
	CDuiStringTable m_StringTable;
	CDuiString m_strDefaultTooltip;
	CControlUI* m_pLastControlGesture;
};

} // namespace DuiLib

#endif // __UIMANAGER_H__
