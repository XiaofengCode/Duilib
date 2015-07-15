#ifndef __UIMENU_H__
#define __UIMENU_H__

#pragma once

#include <map>
#include <vector>
namespace DuiLib {

struct ContextMenuParam
{
	// 1: remove all
	// 2: remove the sub menu
	WPARAM wParam;
	HWND hWnd;
};

enum MenuAlignment
{
	eMenuAlignment_Left = 1 << 1,
	eMenuAlignment_Top = 1 << 2,
	eMenuAlignment_Right = 1 << 3,
	eMenuAlignment_Bottom = 1 << 4,
};


enum MenuItemDefaultInfo
{
	ITEM_DEFAULT_HEIGHT = 30,		//ÿһ��item��Ĭ�ϸ߶ȣ�ֻ����״����ʱ�Զ��壩
	ITEM_DEFAULT_WIDTH = 150,		//���ڵ�Ĭ�Ͽ��

	ITEM_DEFAULT_ICON_WIDTH = 26,	//Ĭ��ͼ����ռ���
	ITEM_DEFAULT_ICON_SIZE = 16,	//Ĭ��ͼ��Ĵ�С

	ITEM_DEFAULT_EXPLAND_ICON_WIDTH = 20,	//Ĭ���¼��˵���չͼ����ռ���
	ITEM_DEFAULT_EXPLAND_ICON_SIZE = 9,		//Ĭ���¼��˵���չͼ��Ĵ�С

	DEFAULT_LINE_LEFT_INSET = ITEM_DEFAULT_ICON_WIDTH + 3,	//Ĭ�Ϸָ��ߵ���߾�
	DEFAULT_LINE_RIGHT_INSET = 7,	//Ĭ�Ϸָ��ߵ��ұ߾�
	DEFAULT_LINE_HEIGHT = 6,		//Ĭ�Ϸָ�����ռ�߶�
	DEFAULT_LINE_COLOR = 0xFFBCBFC4	//Ĭ�Ϸָ�����ɫ

};

#define WM_MENUCLICK WM_USER + 121  //�������հ�ť��������Ϣ


///////////////////////////////////////////////
class ReceiverImplBase;
class ObserverImplBase
{
public:
	virtual void AddReceiver(ReceiverImplBase* receiver) = 0;
	virtual void RemoveReceiver(ReceiverImplBase* receiver) = 0;
	virtual BOOL RBroadcast(ContextMenuParam param) = 0;
};
/////////////////////////////////////////////////
class ReceiverImplBase
{
public:
	virtual void AddObserver(ObserverImplBase* observer) = 0;
	virtual void RemoveObserver() = 0;
	virtual BOOL Receive(ContextMenuParam param) = 0;
};
/////////////////////////////////////////////////

class ReceiverImpl;
class UILIB_API ObserverImpl : public ObserverImplBase
{
	friend class Iterator;
public:
	ObserverImpl():
	m_pMainWndPaintManager(NULL),
	m_pMenuCheckInfo(NULL)
	{
		pReceivers_ = new ReceiversVector;
	}

	~ObserverImpl()
	{
		if (pReceivers_ != NULL)
		{
			delete pReceivers_;
			pReceivers_ = NULL;
		}
		
	}

	virtual void AddReceiver(ReceiverImplBase* receiver)
	{
		if (receiver == NULL)
			return;

		pReceivers_->push_back(receiver);
		receiver->AddObserver(this);
	}

	virtual void RemoveReceiver(ReceiverImplBase* receiver)
	{
		if (receiver == NULL)
			return;

		ReceiversVector::iterator it = pReceivers_->begin();
		for (; it != pReceivers_->end(); ++it)
		{
			if (*it == receiver)
			{
				pReceivers_->erase(it);
				break;
			}
		}
	}

	virtual BOOL RBroadcast(ContextMenuParam param)
	{
		ReceiversVector::reverse_iterator it = pReceivers_->rbegin();
		for (; it != pReceivers_->rend(); ++it)
		{
			(*it)->Receive(param);
		}

		return BOOL();
	}


	class Iterator
	{
		ObserverImpl & _tbl;
		DWORD index;
		ReceiverImplBase* ptr;
	public:
		Iterator( ObserverImpl & table )
			: _tbl( table ), index(0), ptr(NULL)
		{}

		Iterator( const Iterator & v )
			: _tbl( v._tbl ), index(v.index), ptr(v.ptr)
		{}

		ReceiverImplBase* next()
		{
			if ( index >= _tbl.pReceivers_->size() )
				return NULL;

			for ( ; index < _tbl.pReceivers_->size(); )
			{
				ptr = (*(_tbl.pReceivers_))[ index++ ];
				if ( ptr )
					return ptr;
			}
			return NULL;
		}
	};

	virtual void SetManger(CPaintManagerUI* pManager)
	{
		if (pManager != NULL)
			m_pMainWndPaintManager = pManager;
	}

	virtual CPaintManagerUI* GetManager() const
	{
		return m_pMainWndPaintManager;
	}

	virtual void SetMenuCheckInfo(std::map<CDuiString,bool>* pInfo)
	{
		if (pInfo != NULL)
			m_pMenuCheckInfo = pInfo;
		else
			m_pMenuCheckInfo = NULL;
	}

	virtual std::map<CDuiString,bool>* GetMenuCheckInfo() const
	{
		return m_pMenuCheckInfo;
	}

public:
	typedef std::vector<ReceiverImplBase*> ReceiversVector;
	ReceiversVector *pReceivers_;

	CPaintManagerUI* m_pMainWndPaintManager;
	std::map<CDuiString,bool>* m_pMenuCheckInfo;
};

////////////////////////////////////////////////////
class UILIB_API ReceiverImpl : public ReceiverImplBase
{
public:
	ReceiverImpl()
	{
		pObservers_ = new ObserversVector;
	}

	~ReceiverImpl()
	{
		if (pObservers_ != NULL)
		{
			delete pObservers_;
			pObservers_ = NULL;
		}
	}

	virtual void AddObserver(ObserverImplBase* observer)
	{
		pObservers_->push_back(observer);
	}

	virtual void RemoveObserver()
	{
		ObserversVector::iterator it = pObservers_->begin();
		for (; it != pObservers_->end(); ++it)
		{
			(*it)->RemoveReceiver(this);
		}
	}

	virtual BOOL Receive(ContextMenuParam param)
	{
		return BOOL();
	}

protected:
	typedef std::vector<ObserverImplBase*> ObserversVector;
	ObserversVector* pObservers_;
};

/////////////////////////////////////////////////////////////////////////////////////
//


class CListUI;
class UILIB_API CMenuUI : public CListUI
{
public:
	CMenuUI();
	virtual ~CMenuUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

	virtual void DoEvent(TEventUI& event);

    virtual bool Add(CControlUI* pControl);
    virtual bool AddAt(CControlUI* pControl, int iIndex);

    virtual int GetItemIndex(CControlUI* pControl) const;
    virtual bool SetItemIndex(CControlUI* pControl, int iIndex);
    virtual bool Remove(CControlUI* pControl);

	virtual SIZE EstimateSize(SIZE szAvailable);

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) ;
};

/////////////////////////////////////////////////////////////////////////////////////
//

class CMenuElementUI;
class UILIB_API CMenuWnd : public CWindowWnd, public ReceiverImpl, public INotifyUI, public IDialogBuilderCallback
{
public:

	static ObserverImpl& GetGlobalContextMenuObserver()
	{
		static ObserverImpl s_context_menu_observer;
		return s_context_menu_observer;
	}

public:
	CMenuWnd();
	~CMenuWnd();

	/*
	 *	@pOwner һ���˵���Ҫָ��������������ǲ˵��ڲ�ʹ�õ�
	 *	@xml	�˵��Ĳ����ļ�
	 *	@point	�˵������Ͻ�����
	 *	@pMainPaintManager	�˵��ĸ����������ָ��
	 *	@pMenuCheckInfo	����˵��ĵ�ѡ�͸�ѡ��Ϣ�ṹָ��
	 *	@dwAlignment		�˵��ĳ���λ�ã�Ĭ�ϳ������������²ࡣ
	 */
    void Init(CMenuElementUI* pOwner, STRINGorID xml, STRINGorID xmlStringTable, LPCTSTR lpszLang, POINT point,
		CPaintManagerUI* pMainPaintManager, std::map<CDuiString,bool>* pMenuCheckInfo = NULL,
		DWORD dwAlignment = eMenuAlignment_Left | eMenuAlignment_Top);
    LPCTSTR GetWindowClassName() const;
    void OnFinalMessage(HWND hWnd);
	void Notify(TNotifyUI& msg);
	CControlUI* CreateControl(LPCTSTR pstrClassName);

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	BOOL Receive(ContextMenuParam param);

public:

	POINT			m_BasedPoint;
	STRINGorID		m_xml;
	STRINGorID		m_xmlStringTable;
	CDuiString		m_strStringTableLang;
    CPaintManagerUI m_pm;
    CMenuElementUI* m_pOwner;
    CMenuUI*	m_pLayout;
	DWORD		m_dwAlignment;	//�˵����뷽ʽ
};

class CListContainerElementUI;
class UILIB_API CMenuElementUI : public CListContainerElementUI
{
	friend CMenuWnd;
public:
    CMenuElementUI();
	~CMenuElementUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);
    void DoPaint(HDC hDC, const RECT& rcPaint);
	void DrawItemText(HDC hDC, const RECT& rcItem);
	SIZE EstimateSize(SIZE szAvailable);

	void DoEvent(TEventUI& event);

	CMenuWnd* GetMenuWnd();
	void CreateMenuWnd();
	
	void SetLineType();
	void SetLineColor(DWORD color);
	DWORD GetLineColor() const;
	void SetLinePadding(RECT rcInset);
	RECT GetLinePadding() const;
	void SetIcon(LPCTSTR strIcon);
	void SetIconSize(LONG cx, LONG cy);
	void DrawItemIcon(HDC hDC, const RECT& rcItem);
	void SetChecked(bool bCheck = true);
	bool GetChecked() const;
	void SetCheckItem(bool bCheckItem = false);
	bool GetCheckItem() const;

	void SetShowExplandIcon(bool bShow);
	void DrawItemExpland(HDC hDC, const RECT& rcItem);

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

protected:
	CMenuWnd*	m_pWindow;

	bool		m_bDrawLine;	//���ָ���
	DWORD		m_dwLineColor;  //�ָ�����ɫ
	RECT		m_rcLinePadding;	//�ָ��ߵ����ұ߾�

	SIZE		m_szIconSize; 	//��ͼ��
	CDuiString	m_strIcon;
	bool		m_bCheckItem;

	bool		m_bShowExplandIcon;
};

} // namespace DuiLib

#endif // __UIMENU_H__
