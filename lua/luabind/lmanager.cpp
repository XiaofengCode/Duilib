#include "duipub.h"
using namespace DuiLib;

namespace lbind
{
	LBIND_DEFINE_STATIC_FUNC(CPaintManagerUI,Quit)
		if (arg.count()>1)
			PostQuitMessage(arg[2].toInt());
		else
			PostQuitMessage(arg[1].toInt());
	LBIND_END_DEFINE_FUNC

		LBIND_DEFINE_FUNC(CPaintManagerUI,GetInstance)
		return L.lreturn(((lua_Integer)pThis->GetInstance()));
	LBIND_END_DEFINE_FUNC

		LBIND_DEFINE_FUNC(CPaintManagerUI,GetInstancePath)
		return L.lreturn(DUI_T2A(pThis->GetInstancePath()).c_str());
	LBIND_END_DEFINE_FUNC

		LBIND_DEFINE_FUNC(CPaintManagerUI,GetCurrentPath)
		return L.lreturn(DUI_T2A(pThis->GetCurrentPath()).c_str());
	LBIND_END_DEFINE_FUNC

		LBIND_DEFINE_FUNC(CPaintManagerUI,GetResourcePath)
		return L.lreturn(DUI_T2A(pThis->GetResourcePath()).c_str());
	LBIND_END_DEFINE_FUNC

		LBIND_DEFINE_FUNC(CPaintManagerUI,GetResourceZip)
		return L.lreturn(DUI_T2A(pThis->GetResourceZip()).c_str());
	LBIND_END_DEFINE_FUNC

		LBIND_DEFINE_FUNC(CPaintManagerUI,IsCachedResourceZip)
		return L.lreturn(pThis->IsCachedResourceZip());
	LBIND_END_DEFINE_FUNC

		LBIND_DEFINE_FUNC(CPaintManagerUI,SetCurrentPath)
		pThis->SetCurrentPath(CDuiString(arg[2].toString()));
	LBIND_END_DEFINE_FUNC

		LBIND_DEFINE_FUNC(CPaintManagerUI,SetResourcePath)
		pThis->SetResourcePath(CDuiString(arg[2].toString()));
	LBIND_END_DEFINE_FUNC

		LBIND_DEFINE_FUNC(CPaintManagerUI,SetResourceZip)
		pThis->SetResourceZip(CDuiString(arg[2].toString()));
	LBIND_END_DEFINE_FUNC

	LBIND_DEFINE_FUNC(CPaintManagerUI,GetHSL)
		short h,s,l;
		pThis->GetHSL(&h,&s,&l);
		return L.lreturn(h,s,l);
	LBIND_END_DEFINE_FUNC

		LBIND_DEFINE_FUNC(CPaintManagerUI,SetHSL)
		pThis->SetHSL(arg[2].toBool(),arg[3].toInt(),arg[4].toInt(),arg[5].toInt());
	LBIND_END_DEFINE_FUNC

		LBIND_DEFINE_FUNC(CPaintManagerUI,ReloadSkin)
		pThis->ReloadSkin();
	LBIND_END_DEFINE_FUNC

		LBIND_DEFINE_FUNC(CPaintManagerUI,LoadPlugin)
		pThis->LoadPlugin(CDuiString(arg[2].toString()));
	LBIND_END_DEFINE_FUNC


}

LBIND_BEGIN_DEFINE_LIB(CPaintManagerUI)
{"quit",lbind::Quit},
{"getInstance",lbind::GetInstance},
{"instancePath",lbind::GetInstancePath},
{"currentPath",lbind::GetCurrentPath},
{"resourcePath",lbind::GetResourcePath},
{"resourceZip",lbind::GetResourceZip},

{"isCachedResourceZip",lbind::IsCachedResourceZip},
{"setCurrentPath",lbind::SetCurrentPath},

{"setResourcePath",lbind::SetResourcePath},
{"setResourceZip",lbind::SetResourceZip},
{"HSL",lbind::GetHSL},
{"setHSL",lbind::SetHSL},
{"reloadSkin",lbind::ReloadSkin},
{"loadPlugin",lbind::LoadPlugin},

LBIND_END_DEFINE_LIB


