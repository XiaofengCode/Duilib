#ifndef UITreeView_h__
#define UITreeView_h__


#pragma once

namespace DuiLib
{
	class CTreeViewUI;
	class CCheckBoxUI;
	class CLabelUI;
	class COptionUI;
	class CDialogBuilder;

	class UILIB_API CTreeNodeUI : public CListContainerElementUI
	{
		friend class CTreeViewUI;
		friend class CDialogBuilder;

	public:
		CTreeNodeUI(CTreeNodeUI* _ParentNode = NULL, CPaintManagerUI* pManager = NULL);
		~CTreeNodeUI(void);

	public:
		LPCTSTR GetClass() const;
		LPVOID	GetInterface(LPCTSTR pstrName);
		void	DoEvent(TEventUI& event);
		void	Invalidate();
		bool	Select(bool bSelect = true);

		bool	Add(CControlUI* _pTreeNodeUI);
		bool	AddAt(CControlUI* pControl, int iIndex);
		bool	Remove(CControlUI* pControl);

		bool	IsExpanded() const;
		bool	Expand(bool bExpand = true, bool bExpandChildren = false);

		void	SetVisibleTag(bool _IsVisible);
		bool	GetVisibleTag();
		void	SetItemText(LPCTSTR pstrValue);
		CDuiString	GetItemText();
		void	CheckBoxSelected(bool _Selected);
		bool	IsCheckBoxSelected() const;
		bool	IsHasChild() const;
		long	GetTreeLevel() const;
		bool	AddChildNode(CTreeNodeUI* _pTreeNodeUI);
		bool	RemoveAt(CTreeNodeUI* _pTreeNodeUI);
		void	SetParentNode(CTreeNodeUI* _pParentTreeNode);
		CTreeNodeUI* GetParentNode() const;
		long	GetCountChild() const;
		void	SetTreeView(CTreeViewUI* _CTreeViewUI);
		CTreeViewUI* GetTreeView();
		CTreeNodeUI* GetChildNode(int _nIndex) const;
		void	SetVisibleFolderBtn(bool _IsVisibled);
		bool	GetVisibleFolderBtn();
		void	SetVisibleCheckBtn(bool _IsVisibled);
		bool	GetVisibleCheckBtn();
		void	SetItemTextColor(DWORD _dwItemTextColor);
		DWORD	GetItemTextColor() const;
		void	SetItemHotTextColor(DWORD _dwItemHotTextColor);
		DWORD	GetItemHotTextColor() const;
		void	SetSelItemTextColor(DWORD _dwSelItemTextColor);
		DWORD	GetSelItemTextColor() const;
		void	SetSelItemHotTextColor(DWORD _dwSelHotItemTextColor);
		DWORD	GetSelItemHotTextColor() const;

		void	SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		
		void	IsAllChildChecked(); //检测所有子项是否都选中 2014.12.11 redrain
		CStdPtrArray GetTreeNodes();

		int			 GetTreeIndex();
		int			 GetNodeIndex();

		// 鼠标提示
		virtual CDuiString GetToolTip() const;
		virtual void SetToolTip(LPCTSTR pstrText);
		virtual void SetToolTipWidth(int nWidth);
		virtual int	  GetToolTipWidth(void);	// 多行ToolTip单行最长宽度

		//CControlUI* FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);
		void SetEnabled(bool bEnable = true);
	private:
		CTreeNodeUI* GetLastNode();
		void CalcLocation(CTreeNodeUI* _pTreeNodeUI);
		void CalcAllChildren();

	protected:
		CHorizontalLayoutUI*	GetTreeNodeHoriznotal() const {return m_pHoriz;}
		CCheckBoxUI*			GetFolderButton() const {return m_pFolderButton;}
		CLabelUI*				GetDottedLine() const {return m_pDottedLine;}
		CCheckBoxUI*			GetCheckBox() const {return m_pCheckBox;}
		COptionUI*				GetItemButton() const {return m_pItemButton;}

	private:
		// Status
		long	m_iTreeLavel;
		bool	m_bIsVisable;
		bool	m_bIsCheckBox;
		bool	m_bExpanded;
		DWORD	m_dwItemTextColor;
		DWORD	m_dwItemHotTextColor;
		DWORD	m_dwSelItemTextColor;
		DWORD	m_dwSelItemHotTextColor;

		// Temp
		bool	m_bExpanding;


		CTreeViewUI*			m_pTreeView;
		CHorizontalLayoutUI*	m_pHoriz;
		CCheckBoxUI*			m_pFolderButton;
		CLabelUI*				m_pDottedLine;
		CCheckBoxUI*			m_pCheckBox;
		COptionUI*				m_pItemButton;
		CLabelUI*				m_pOverFolderButton;

		CTreeNodeUI*			m_pParentTreeNode;

		CStdPtrArray			m_arTreeNodes;
	};

	class UILIB_API CTreeViewUI : public CListUI,public INotifyUI
	{
	public:
		LBIND_CLASS_DEFINE(CTreeViewUI, CListUI);
		CTreeViewUI(void);
		~CTreeViewUI(void);

	public:
		virtual LPCTSTR GetClass() const;
		virtual LPVOID	GetInterface(LPCTSTR pstrName);
		virtual bool Add(CTreeNodeUI* pControl );
		virtual long AddAt(CTreeNodeUI* pControl, int iIndex );
		virtual bool AddAt(CTreeNodeUI* pControl,CTreeNodeUI* _IndexNode);
		virtual bool Remove(CTreeNodeUI* pControl);
		virtual bool RemoveAt(int iIndex);
		virtual void RemoveAll();
		virtual bool OnCheckBoxChanged(void* param);
		virtual bool OnFolderChanged(void* param);
		virtual bool OnDBClickItem(void* param);
		virtual bool SetItemCheckBox(bool _Selected,CTreeNodeUI* _TreeNode = NULL);
		virtual void SetItemExpand(bool _Expanded,CTreeNodeUI* _TreeNode = NULL, bool bExpandChildren = false);
		virtual void Notify(TNotifyUI& msg);
		virtual void SetVisibleFolderBtn(bool _IsVisibled);
		virtual bool GetVisibleFolderBtn();
		virtual void SetVisibleCheckBtn(bool _IsVisibled);
		virtual bool GetVisibleCheckBtn();
		virtual void SetItemMinWidth(UINT _ItemMinWidth);
		virtual UINT GetItemMinWidth();
		virtual void SetItemTextColor(DWORD _dwItemTextColor);
		virtual void SetItemHotTextColor(DWORD _dwItemHotTextColor);
		virtual void SetSelItemTextColor(DWORD _dwSelItemTextColor);
		virtual void SetSelItemHotTextColor(DWORD _dwSelHotItemTextColor);
		
		virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	private:
		UINT m_uItemMinWidth;
		bool m_bVisibleFolderBtn;
		bool m_bVisibleCheckBtn;
	};
}


#endif // UITreeView_h__
