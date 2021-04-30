#ifndef __UICONTROL_H__
#define __UICONTROL_H__

#pragma once
namespace DuiLib {
#define DUI_ATTR_STATUS_FOCUSED		_T("focused")
#define DUI_ATTR_STATUS_DISABLED	_T("disabled")
#define DUI_ATTR_STATUS_HOT			_T("hot")
#define DUI_ATTR_STATUS_PUSHED		_T("pushed")
#define DUI_ATTR_STATUS_SELECTED	_T("selected")

#define DUI_ATTR_POS_FOCUSDOT		_T("focusdot")
#define DUI_ATTR_POS_BK				_T("bk")
#define DUI_ATTR_POS_FORE			_T("fore")
#define DUI_ATTR_POS_NORMAL			_T("normal")
#define DUI_ATTR_POS_BORDER			_T("border")

#define DUI_ATTR_TEXT				_T("text")

#define DUI_ATTR_WIDTH				_T("width")
#define DUI_ATTR_HEIGHT				_T("height")
#define DUI_ATTR_COLOR				_T("color")
#define DUI_ATTR_COLOR2				_T("color2")
#define DUI_ATTR_COLOR3				_T("color3")
#define DUI_ATTR_POS				_T("pos")
#define DUI_ATTR_PADDING			_T("padding")
#define DUI_ATTR_SIZE				_T("size")
#define DUI_ATTR_ROUND				_T("round")
#define DUI_ATTR_FONT				_T("font")
#define DUI_ATTR_TOOLTIP			_T("tooltip")
#define DUI_ATTR_IMAGE				_T("image")
#define DUI_ATTR_TIPVALUE			_T("tipvalue")
//#define DUI_ATTR_USERDATA			_T("userdata")
//#define DUI_ATTR_ENABLED			_T("enabled")
//#define DUI_ATTR_KEYBOARD			_T("keyboard")
//#define DUI_ATTR_VISIBLE			_T("visible")
//#define DUI_ATTR_FLOAT				_T("float")
//#define DUI_ATTR_SHORTCUT			_T("shortcut")
//#define DUI_ATTR_MENU				_T("menu")
//#define DUI_ATTR_VIRTUALWND			_T("virtualwnd")
//#define DUI_ATTR_NAME				_T("name")

/////////////////////////////////////////////////////////////////////////////////////
//
#define TIMER_BK_GIF	1000
typedef CControlUI* (CALLBACK* FINDCONTROLPROC)(CControlUI*, LPVOID);
class UILIB_API CContainerUI;
class UILIB_API CControlUI
{
public:
    CControlUI();
    virtual ~CControlUI();

public:
    virtual CDuiString GetName() const;
    virtual void SetName(LPCTSTR pstrName);
    virtual LPCTSTR GetClass() const;
    virtual LPVOID GetInterface(LPCTSTR pstrName);
    virtual UINT GetControlFlags() const;

    virtual bool Activate();
    virtual CPaintManagerUI* GetManager() const;
    virtual void SetManager(CPaintManagerUI* pManager, CControlUI* pParent, bool bInit = true);
    virtual CContainerUI* GetParent() const;

    // �ı����
    virtual CDuiString GetText() const;
    virtual void SetText(LPCTSTR pstrText);

	virtual	double GetScaleDpi();
	virtual	void SetTextColor(DWORD dwTextColor);
	virtual	DWORD GetTextColor() const;

    // ͼ�����
    DWORD GetBkColor() const;
    void SetBkColor(DWORD dwBackColor);
    DWORD GetBkColor2() const;
    void SetBkColor2(DWORD dwBackColor);
    DWORD GetBkColor3() const;
    void SetBkColor3(DWORD dwBackColor);
    const CDuiImage GetBkImage();
    void SetBkImage(LPCTSTR pStrImage);
	DWORD GetFocusBorderColor() const;
	void SetFocusBorderColor(DWORD dwBorderColor);
	DWORD GetDisabledBorderColor() const;
	void SetDisabledBorderColor(DWORD dwBorderColor);	
    bool IsColorHSL() const;
    void SetColorHSL(bool bColorHSL);
    SIZE GetBorderRound() const;
	void SetBorderRound(SIZE cxyRound);
	bool DrawImage(HDC hDC, LPCTSTR pStrImage, LPCTSTR pStrModify = NULL);
	bool DrawImage(HDC hDC, const CDuiImageItem& img);
	bool DrawImage(HDC hDC, const CDuiImage& img);

	//�߿����
	int GetBorderSize() const;
	void SetBorderSize(int nSize);
	DWORD GetBorderColor() const;
	void SetBorderColor(DWORD dwBorderColor);
	DWORD GetFocusDotColor() const;
	void SetFocusDotColor(DWORD dwColor);

	void SetBorderSize(RECT rc);
	int GetLeftBorderSize() const;
	void SetLeftBorderSize(int nSize);
	int GetTopBorderSize() const;
	void SetTopBorderSize(int nSize);
	int GetRightBorderSize() const;
	void SetRightBorderSize(int nSize);
	int GetBottomBorderSize() const;
	void SetBottomBorderSize(int nSize);
	int GetBorderStyle() const;
	void SetBorderStyle(int nStyle);

	// λ�����
	virtual DWORD GetStatus();
	virtual CDuiString GetStatusString(DWORD dwStatus = 0, int nIgnorStatus = 0);
	virtual DWORD GetStatusColor(LPCTSTR lpszAttr);
	virtual CDuiImage GetStatusImage(LPCTSTR lpszAttr);
	virtual RECT GetStatusRect(LPCTSTR lpszAttr);
	virtual int GetStatusInt(LPCTSTR lpszAttr);

    virtual const RECT& GetPos() const;
    virtual void SetPos(RECT rc);
    virtual int GetWidth() const;
    virtual int GetHeight() const;
    virtual int GetX() const;
    virtual int GetY() const;
    virtual RECT GetPadding() const;
    virtual void SetPadding(RECT rcPadding); // ������߾࣬���ϲ㴰�ڻ���
    virtual SIZE GetFixedXY() const;         // ʵ�ʴ�Сλ��ʹ��GetPos��ȡ������õ�����Ԥ��Ĳο�ֵ
    virtual void SetFixedXY(SIZE szXY);      // ��floatΪtrueʱ��Ч
    virtual int GetFixedWidth() const;       // ʵ�ʴ�Сλ��ʹ��GetPos��ȡ������õ�����Ԥ��Ĳο�ֵ
    virtual void SetFixedWidth(int cx);      // Ԥ��Ĳο�ֵ
    virtual int GetFixedHeight() const;      // ʵ�ʴ�Сλ��ʹ��GetPos��ȡ������õ�����Ԥ��Ĳο�ֵ
    virtual void SetFixedHeight(int cy);     // Ԥ��Ĳο�ֵ
    virtual int GetMinWidth() const;
    virtual void SetMinWidth(int cx);
    virtual int GetMaxWidth() const;
    virtual void SetMaxWidth(int cx);
    virtual int GetMinHeight() const;
    virtual void SetMinHeight(int cy);
    virtual int GetMaxHeight() const;
    virtual void SetMaxHeight(int cy);
    virtual void SetRelativePos(SIZE szMove,SIZE szZoom);
    virtual void SetRelativeParentSize(SIZE sz);
    virtual TRelativePosUI GetRelativePos() const;
    virtual bool IsRelativePos() const;

    // �����ʾ
    virtual CDuiString GetToolTip() const;
    virtual void SetToolTip(LPCTSTR pstrText);
	virtual void SetToolTipWidth(int nWidth);
	virtual int	  GetToolTipWidth(void);	// ����ToolTip��������

    // ��ݼ�
    virtual TCHAR GetShortcut() const;
    virtual void SetShortcut(TCHAR ch);

    // �˵�
    virtual bool IsContextMenuUsed() const;
    virtual void SetContextMenuUsed(bool bMenuUsed);

    // �û�����
    virtual const CDuiString& GetUserData() const; // �������������û�ʹ��
    virtual void SetUserData(LPCTSTR pstrText); // �������������û�ʹ��
    virtual UINT_PTR GetTag() const; // �������������û�ʹ��
    virtual void SetTag(UINT_PTR pTag); // �������������û�ʹ��

    // һЩ��Ҫ������
    virtual bool IsVisible() const;
    virtual void SetVisible(bool bVisible = true);
    virtual void SetInternVisible(bool bVisible = true); // �����ڲ����ã���ЩUIӵ�д��ھ������Ҫ��д�˺���
    virtual bool IsEnabled() const;
    virtual void SetEnabled(bool bEnable = true);
    virtual bool IsMouseEnabled() const;
    virtual void SetMouseEnabled(bool bEnable = true);
    virtual bool IsKeyboardEnabled() const;
    virtual void SetKeyboardEnabled(bool bEnable = true);
    virtual bool IsFocused() const;
    virtual void SetFocus();
    virtual bool IsFloat() const;
    virtual void SetFloat(bool bFloat = true);

    virtual CControlUI* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);

    void Invalidate();
    bool IsUpdateNeeded() const;
    void NeedUpdate();
    void NeedParentUpdate();
    DWORD GetAdjustColor(DWORD dwColor);

    virtual void Init();
    virtual void DoInit();

    virtual void Event(TEventUI& event);
    virtual void DoEvent(TEventUI& event);

	virtual CDuiString GetAttribute(LPCTSTR pstrName) const;
    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
    void ApplyAttributeList(LPCTSTR pstrList);

    virtual SIZE EstimateSize(SIZE szAvailable);

    virtual void DoPaint(HDC hDC, const RECT& rcPaint);
	virtual void PaintBkColor(HDC hDC);
    virtual void PaintBkImage(HDC hDC);
	virtual void PaintStatusImage(HDC hDC);
	virtual void PaintNormalColor(HDC hDC);
	virtual void PaintNormalImage(HDC hDC);
	virtual void PaintForeColor(HDC hDC);
	virtual void PaintForeImage(HDC hDC);
    virtual void PaintText(HDC hDC);
	virtual void PaintBorder(HDC hDC);
	virtual void PaintFocusedDot(HDC hDC);

    virtual void DoPostPaint(HDC hDC, const RECT& rcPaint);

	//���ⴰ�ڲ���
	void SetVirtualWnd(LPCTSTR pstrValue);
	CDuiString GetVirtualWnd(bool bGetParent = true) const;
	int GetVirtualWnd(CDuiStringArray& arVirtualWnd) const;

 	virtual void  OnTimer( UINT_PTR idEvent );

	virtual CDuiString GetStyle();
	virtual void SetStyle(LPCTSTR lpszStyle);

	DWORD GetXmlOffset(){return m_dwOffsetInXml;}
	void SetXmlOffset(DWORD dwOffset){m_dwOffsetInXml = dwOffset;}
	LPCTSTR GetXmlFile(){return m_strXmlFileName;}
	void SetXmlFile(LPCTSTR lpszFile){m_strXmlFileName = lpszFile;}
	LPCTSTR LoadString( LPCTSTR lpszID );
public:
    CEventSource OnInit;
    CEventSource OnDestroy;
    CEventSource OnSize;
    CEventSource OnEvent;
    CEventSource OnNotify;

protected:

    CPaintManagerUI* m_pManager;
    CContainerUI* m_pParent;
	CDuiString m_sVirtualWnd;
    CDuiString m_sName;
    bool m_bUpdateNeeded;
    bool m_bMenuUsed;
    RECT m_rcItem;
    //RECT m_rcPadding;
    SIZE m_cXY;
    SIZE m_cxyMin;
    SIZE m_cxyMax;
    bool m_bVisible;
    bool m_bInternVisible;
    //bool m_bEnabled;
    bool m_bMouseEnabled;
	bool m_bKeyboardEnabled ;
    //bool m_bFocused;
    bool m_bFloat;
    bool m_bSetPos; // ��ֹSetPosѭ������
    TRelativePosUI m_tRelativePos;

    CDuiString m_sText;
    CDuiString m_sToolTip;
    TCHAR m_chShortcut;
    CDuiString m_sUserData;
    UINT_PTR m_pTag;

    bool m_bColorHSL;
    //int m_nBorderSize;
	int m_nBorderStyle;
	int m_nTooltipWidth;
    SIZE m_cxyBorderRound;
    RECT m_rcPaint;
	//RECT m_rcBorderSize;
	bool m_bShowFocusDot;

	//Gif
	CGifImage		m_gifBk;
// 	int  m_nWidthScale;
// 	int  m_nHeightScale;
	CDuiString	m_strStyle;

	DWORD m_dwStatus;
	CAttributeManager m_attrs;
	DWORD m_dwOffsetInXml;//����ʹ��
	CDuiString m_strXmlFileName;//����ʹ��
};

} // namespace DuiLib

#endif // __UICONTROL_H__
