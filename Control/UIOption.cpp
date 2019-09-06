#include "duipub.h"
#include "UIOption.h"

namespace DuiLib
{
	COptionUI::COptionUI() : m_bSelected(false)
	{
	}

	COptionUI::~COptionUI()
	{
		CDuiString strGroup = GetGroup();
		if( !strGroup.IsEmpty() && m_pManager )
			m_pManager->RemoveOptionGroup(strGroup, this);
	}

	LPCTSTR COptionUI::GetClass() const
	{
		return _T("OptionUI");
	}

	LPVOID COptionUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_OPTION) == 0 ) return static_cast<COptionUI*>(this);
		return CButtonUI::GetInterface(pstrName);
	}

	void COptionUI::SetManager(CPaintManagerUI* pManager, CControlUI* pParent, bool bInit)
	{
		CButtonUI::SetManager(pManager, pParent, bInit);
		CDuiString strGroup = GetGroup();
		if( bInit && !strGroup.IsEmpty() )
		{
			if (m_pManager)
				m_pManager->AddOptionGroup(strGroup, this);
		}
	}

	CDuiString COptionUI::GetGroup() const
	{
		if (m_sGroupName.GetLength() == 0)
		{
			return _T("");
		}
		CDuiStringArray arParent;
		GetVirtualWnd(arParent);
		CDuiString strPrefix;
		for (int i = 0; i < arParent.GetSize(); i++)
		{
			strPrefix += arParent[i];
			strPrefix += _T(".");
		}
		return strPrefix + m_sGroupName;
	}

	void COptionUI::SetGroup(LPCTSTR pStrGroupName)
	{
		if( pStrGroupName == NULL )
		{
			if( m_sGroupName.IsEmpty() )
				return;
			m_sGroupName.Empty();
		}
		else {
			if( m_sGroupName == pStrGroupName )
				return;
			if (!m_sGroupName.IsEmpty() && m_pManager)
				m_pManager->RemoveOptionGroup(GetGroup(), this);
			m_sGroupName = pStrGroupName;
		}

		CDuiString strGroup = GetGroup();
		if( !strGroup.IsEmpty() )
		{
			if (m_pManager) m_pManager->AddOptionGroup(strGroup, this);
		}
		else
		{
			if (m_pManager) m_pManager->RemoveOptionGroup(strGroup, this);
		}

		Selected(m_bSelected);
	}

	bool COptionUI::IsSelected() const
	{
		return m_bSelected;
	}

	void COptionUI::Selected(bool bSelected)
	{
//		if( m_bSelected == bSelected ) return;
		m_bSelected = bSelected;
		if( m_bSelected ) m_dwStatus |= UISTATE_SELECTED;
		else m_dwStatus &= ~UISTATE_SELECTED;

		if( m_pManager != NULL ) 
		{
			CDuiString strGroup = GetGroup();
			if( !strGroup.IsEmpty() ) 
			{
				if( m_bSelected ) 
				{
					CStdPtrArray* aOptionGroup = m_pManager->GetOptionGroup(strGroup);
					for( int i = 0; i < aOptionGroup->GetSize(); i++ ) 
					{
						COptionUI* pControl = static_cast<COptionUI*>(aOptionGroup->GetAt(i));
						if( pControl != this ) 
						{
							pControl->Selected(false);
						}
					}
					m_pManager->SendNotify(this, DUI_MSGTYPE_SELECTCHANGED);
				}
				else
				{
					m_pManager->SendNotify(this, DUI_MSGTYPE_UNSELECTED);
				}
			}
			else 
			{
				m_pManager->SendNotify(this, DUI_MSGTYPE_SELECTCHANGED);
			}
		}

		Invalidate();
	}

	bool COptionUI::Activate()
	{
		if( !CButtonUI::Activate() ) return false;
		if( !m_sGroupName.IsEmpty() ) Selected(true);
		else Selected(!m_bSelected);

		return true;
	}

	void COptionUI::SetEnabled(bool bEnable)
	{
		CButtonUI::SetEnabled(bEnable);
// 		if( !IsEnabled() ) {
// 			if( m_bSelected ) m_uButtonState = UISTATE_SELECTED;
// 			else m_uButtonState = 0;
// 		}
	}

	SIZE COptionUI::EstimateSize(SIZE szAvailable)
	{
		if( GetFixedHeight() == 0 )
			return CDuiSize(GetFixedWidth(), m_pManager->GetFontInfo(GetFont())->tm.tmHeight + 8);
		return CButtonUI::EstimateSize(szAvailable);
	}

	void COptionUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		if( _tcsicmp(pstrName, _T("group")) == 0 ) SetGroup(pstrValue);
		else if( _tcsicmp(pstrName, _T("selected")) == 0 ) Selected(_tcsicmp(pstrValue, _T("true")) == 0);
		else CButtonUI::SetAttribute(pstrName, pstrValue);
	}

}