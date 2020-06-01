#ifndef __UIOPTION_H__
#define __UIOPTION_H__

#pragma once

namespace DuiLib
{
	class UILIB_API COptionUI : public CButtonUI
	{
	public:
		COptionUI();
		~COptionUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void SetManager(CPaintManagerUI* pManager, CControlUI* pParent, bool bInit = true);

		bool Activate();
		void SetEnabled(bool bEnable = true);

		CDuiString GetGroup() const;
		void SetGroup(LPCTSTR pStrGroupName = NULL);
		bool IsSelected() const;
		virtual void Selected(bool bSelected);

		SIZE EstimateSize(SIZE szAvailable);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	protected:
		bool			m_bSelected;
		CDuiString		m_sGroupName;
		CDuiString		m_sManagerGroupName;

	};

} // namespace DuiLib

#endif // __UIOPTION_H__