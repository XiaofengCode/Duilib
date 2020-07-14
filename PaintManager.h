#pragma once


inline void PaintManager_SetInstance(HINSTANCE hInst)
{
	CPaintManagerUI::SetInstance(hInst);
}

inline void PaintManager_SetResourcePath(LPCTSTR lpszResourcePath)
{
	CPaintManagerUI::SetResourcePath(lpszResourcePath);
}


#define DECLEAR_CLASS_METHOD(retype, cname, method, ...)\
retype C##cname##UI_##method(C##cname##UI* pThis, __VA_ARGS__)

#define IMPL_CLASS_METHOD(retype, cname, method, fields, ...)\
UILIB_API inline retype cname##_##method(cname* pThis, __VA_ARGS__){return pThis->method fields;}

IMPL_CLASS_METHOD(void, CPaintManagerUI, Init, (hWnd), HWND hWnd);
IMPL_CLASS_METHOD(void, CPaintManagerUI, NeedUpdate, ());
IMPL_CLASS_METHOD(HDC, CPaintManagerUI, GetPaintDC, ());
IMPL_CLASS_METHOD(HWND, CPaintManagerUI, GetPaintWindow, ());
IMPL_CLASS_METHOD(HWND, CPaintManagerUI, GetTooltipWindow, ());

