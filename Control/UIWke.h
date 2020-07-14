#pragma once
#include "wke.h"
using namespace DuiLib;
typedef struct  
{
	wkeString url;
	wkeNavigationType navType;
	wkeWindowFeatures windowFeatures;
}WKE_CREATEVIEW_PARAM;
class CWkeUI : public CContainerUI
{
public:
	CWkeUI(void);
	~CWkeUI(void);
	virtual void DoInit();
	virtual void SetVisible(bool bVisible = true);
	virtual void SetInternVisible(bool bVisible = true);
	virtual void SetPos(RECT rc);
	virtual void SetAttribute( LPCTSTR pstrName, LPCTSTR pstrValue );

	BOOL Attach(HWND hWndNew);
	HWND Detach();
	HWND GetHWND();

	
protected:
	static int m_nWkeInitedCount;
	CDuiString m_strUrl;
	wkeWebView webview;
};

