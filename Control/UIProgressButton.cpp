#include "..\duipub.h"
#include "UIProgressButton.h"

namespace DuiLib
{
	CProgressButtonUI::CProgressButtonUI()
	{
		m_bButton = true;
	}

	LPCTSTR CProgressButtonUI::GetClass() const
	{
		return _T("ProgressButtonUI");
	}

	LPVOID CProgressButtonUI::GetInterface(LPCTSTR pstrName)
	{
		if( _tcscmp(pstrName, DUI_CTR_PROGRESSBUTTON) == 0 ) return static_cast<CProgressButtonUI*>(this);
		return __super::GetInterface(pstrName);
	}
}
