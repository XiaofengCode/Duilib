#include "..\duipub.h"
#include "UIWke.h"


int CWkeUI::m_nWkeInitedCount = false;
CWkeUI::CWkeUI(void)
{
	if (!m_nWkeInitedCount)
	{
		if (wkeInitialize())
		{
			m_nWkeInitedCount = 1;
		}
	}
	else
	{
		m_nWkeInitedCount++;
	}
}


CWkeUI::~CWkeUI(void)
{
	if (!m_nWkeInitedCount)
	{
		return;
	}
	m_nWkeInitedCount--;
	if (!m_nWkeInitedCount)
	{
		wkeFinalize();
		return;
	}
}

// 回调：点击了关闭、返回 true 将销毁窗口，返回 false 什么都不做。
bool handleWindowClosing(wkeWebView webWindow, void* param)
{
	CWkeUI* app = (CWkeUI*)param;
	app->GetManager()->SendNotify(app, _T("wke_closing"), 0, 0, false);
	return IDYES == MessageBoxW(NULL, L"确定要退出程序吗？", L"wkexe", MB_YESNO | MB_ICONQUESTION);
}

// 回调：窗口已销毁
void handleWindowDestroy(wkeWebView webWindow, void* param)
{
	CWkeUI* app = (CWkeUI*)param;
	app->GetManager()->SendNotify(app, _T("wke_destroy"), 0, 0, false);
}

// 回调：文档加载成功
void handleDocumentReady(wkeWebView webWindow, void* param)
{
	CWkeUI* app = (CWkeUI*)param;
	app->GetManager()->SendNotify(app, _T("wke_ready"), 0, 0, false);
	//wkeShowWindow(webWindow, true);
}

// 回调：页面标题改变
void handleTitleChanged(wkeWebView webWindow, void* param, const wkeString title)
{
	CWkeUI* app = (CWkeUI*)param;
	app->GetManager()->SendNotify(app, _T("wke_titlechanged"), 0, 0, false);
	//wkeSetWindowTitleW(webWindow, wkeGetStringW(title));
}

// 回调：创建新的页面，比如说调用了 window.open 或者点击了 <a target="_blank" .../>
wkeWebView onCreateView(wkeWebView webWindow, void* param, wkeNavigationType navType, const wkeString url, const wkeWindowFeatures* features)
{
	CWkeUI* app = (CWkeUI*)param;

	WKE_CREATEVIEW_PARAM cv;
	cv.navType = navType;
	cv.url = url;
	cv.windowFeatures = *features;
	app->GetManager()->SendNotify(app, _T("wke_newpage"), (WPARAM)&cv);
	
	return NULL;
// 	wkeWebView newWindow = wkeCreateWebWindow(WKE_WINDOW_TYPE_POPUP, NULL, features->x, features->y, features->width, features->height);
// 	wkeShowWindow(newWindow, true);
// 	return newWindow;
}

const char kPreHead[] = "http://dui.res/";
bool onLoadUrlBegin(wkeWebView webView, void* param, const char* url, void *job)
{
	CWkeUI* app = (CWkeUI*)param;
	app->GetManager()->SendNotify(app, _T("wke_urlbegin"), (WPARAM)url);
	const utf8* decodeURL = wkeUtilDecodeURLEscape(url);
	if (!decodeURL)
		return false;
	std::string urlString(decodeURL);
	std::string localPath = urlString.substr(sizeof(kPreHead) - 1);

	CDuiBuffer buffer;
	DuiReadResourceFileData(DuiUtf8ToUtf16(localPath.c_str()).c_str(), buffer);
	wkeNetSetData(job, buffer, buffer.GetSize());
	return true;
}

void onLoadUrlEnd(wkeWebView webView, void* param, const char *url, void *job, void* buf, int len)
{
	CWkeUI* app = (CWkeUI*)param;
	app->GetManager()->SendNotify(app, _T("wke_urlend"), (WPARAM)url);
	return;
}

void WKE_CALL_TYPE onDidCreateScriptContextCallback(wkeWebView webView, void* param, wkeWebFrameHandle frameId, void* context, int extensionGroup, int worldId)
{

}

#pragma comment(lib, "Msimg32.lib")
void WKE_CALL_TYPE onPaint(wkeWebView webView, void* param, const HDC hdc, int x, int y, int cx, int cy)
{
	CWkeUI* app = (CWkeUI*)param;
	//TransparentBlt()
	auto pos = app->GetPos();
	TransparentBlt(app->GetManager()->GetPaintDC(), pos.left + x, pos.top + y, cx, cy, hdc, x, y, cx, cy, app->GetBkColor() & 0xFFFFFF);
}

void CWkeUI::DoInit()
{
	if (!wkeCreateWebWindow)
	{
		return;
	}
	wkeWindowCreateInfo info = {0};
	info.color = GetBkColor();
	info.x = 0;
	info.y = 0;
	info.width = 1;
	info.height = 1;
	info.parent = m_pManager->GetPaintWindow();
	info.style = WS_POPUP| WS_CLIPCHILDREN;
	info.styleEx = WS_EX_LAYERED| WS_EX_TRANSPARENT;
	webview = wkeCreateWebCustomWindow(&info);
	wkeSetTransparent(webview, true);
	wkeOnPaintUpdated(webview, onPaint, this);
	//webview = wkeCreateWebWindow(WKE_WINDOW_TYPE_TRANSPARENT, m_pManager->GetPaintWindow(), 0, 0, 640, 480);
	wkeSetWindowTitleW(webview, m_sText);

	wkeOnDidCreateScriptContext(webview, onDidCreateScriptContextCallback, this);
	wkeOnWindowClosing(webview, handleWindowClosing, this);
	wkeOnWindowDestroy(webview, handleWindowDestroy, this);
	wkeOnDocumentReady(webview, handleDocumentReady, this);
	wkeOnTitleChanged(webview, handleTitleChanged, this);
	wkeOnCreateView(webview, onCreateView, this);
	wkeOnLoadUrlBegin(webview, onLoadUrlBegin, this);
	wkeOnLoadUrlEnd(webview, onLoadUrlEnd, this);

	wkeMoveToCenter(webview);

	wkeLoadURL(webview, DuiUtf16ToUtf8(m_strUrl).c_str());


	//wkeShowWindow(webview, IsVisible());
}

void CWkeUI::SetVisible(bool bVisible /*= true*/)
{
	__super::SetVisible(bVisible);
	//wkeShowWindow(webview, IsVisible());
}

void CWkeUI::SetInternVisible(bool bVisible /*= true*/)
{
	__super::SetInternVisible(bVisible);
	//wkeShowWindow(webview, IsVisible());
}

void CWkeUI::SetPos(RECT rc)
{
	__super::SetPos(rc);
	if (!wkeMoveWindow)
	{
		return;
	}
	POINT pt = {rc.left, rc.top};
	ClientToScreen(m_pManager->GetPaintWindow(), &pt);
	wkeMoveWindow(webview, pt.x, pt.y, rc.right - rc.left, rc.bottom - rc.top);
}

void CWkeUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	if (_tcsicmp(pstrName, _T("url")) == 0)
	{
		m_strUrl = pstrValue;
	}
	__super::SetAttribute(pstrName, pstrValue);
}

