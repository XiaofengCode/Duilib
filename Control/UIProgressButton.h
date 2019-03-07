#ifndef __UIPROGRESSBUTTON_H__
#define __UIPROGRESSBUTTON_H__

#pragma once

namespace DuiLib
{
	//现在已经在CProgressUI实现了该类的功能，此处不需要了，为了兼容
	class UILIB_API CProgressButtonUI : public CProgressUI
	{
	public:
		CProgressButtonUI();
		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
	};

} // namespace DuiLib

#endif // __UIPROGRESSBUTTON_H__
