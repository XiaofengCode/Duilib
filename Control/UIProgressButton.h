#ifndef __UIPROGRESSBUTTON_H__
#define __UIPROGRESSBUTTON_H__

#pragma once

namespace DuiLib
{
	//�����Ѿ���CProgressUIʵ���˸���Ĺ��ܣ��˴�����Ҫ�ˣ�Ϊ�˼���
	class UILIB_API CProgressButtonUI : public CProgressUI
	{
	public:
		CProgressButtonUI();
		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
	};

} // namespace DuiLib

#endif // __UIPROGRESSBUTTON_H__
