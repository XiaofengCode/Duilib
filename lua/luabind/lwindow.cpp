#include "duipub.h"
using namespace DuiLib;

namespace lbind
{
	LBIND_DEFINE_FUNC(WindowImplBase,ShowWindow)
		if (arg.count()>2)
			pThis->ShowWindow(arg[2].toBool(),arg[3].toBool());
		else
			pThis->ShowWindow(arg[2].toBool());
	LBIND_END_DEFINE_FUNC

		LBIND_DEFINE_FUNC(WindowImplBase,ShowMin)
		::ShowWindow(pThis->GetHWND(),SW_MINIMIZE);
	LBIND_END_DEFINE_FUNC

		LBIND_DEFINE_FUNC(WindowImplBase,ShowMax)
		::ShowWindow(pThis->GetHWND(),SW_MAXIMIZE);
	LBIND_END_DEFINE_FUNC

		LBIND_DEFINE_FUNC(WindowImplBase,ShowRestore)
		::ShowWindow(pThis->GetHWND(),SW_RESTORE);
	LBIND_END_DEFINE_FUNC

		LBIND_DEFINE_FUNC(WindowImplBase,Close)
		pThis->Close();
	LBIND_END_DEFINE_FUNC

		LBIND_DEFINE_FUNC(WindowImplBase,CenterWindow)
		pThis->CenterWindow();
	LBIND_END_DEFINE_FUNC

		LBIND_DEFINE_FUNC(WindowImplBase,ResizeClient)
		pThis->ResizeClient(arg[2].toInt(),arg[3].toInt());
	LBIND_END_DEFINE_FUNC


		LBIND_DEFINE_FUNC(WindowImplBase,GetDlgItem)
		CControlUI* ctl=pThis->GetDlgItem(CDuiString(arg[2].toString()));
	if(ctl)
		return L.lreturn(ctl->_lbindCToLua(&L));
	LBIND_END_DEFINE_FUNC

}

LBIND_BEGIN_DEFINE_LIB(WindowImplBase)
{"showWindow",lbind::ShowWindow},
{"showMin",lbind::ShowMin},
{"showMax",lbind::ShowMax},
{"showRestore",lbind::ShowRestore},

{"close",lbind::Close},
{"centerWindow",lbind::CenterWindow},
{"resizeClient",lbind::ResizeClient},
{"getDlgItem",lbind::GetDlgItem},
LBIND_END_DEFINE_LIB


















