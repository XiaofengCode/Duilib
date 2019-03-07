#include "duipub.h"
#include "lutil.h"

using namespace DuiLib;

namespace lbind
{
LBIND_DEFINE_FUNC(CLabelUI,SetTextStyle)
	pThis->SetTextStyle(arg[2].toInt());
LBIND_END_DEFINE_FUNC

LBIND_DEFINE_FUNC(CLabelUI,GetTextStyle)
	return L.lreturn(pThis->GetTextStyle());
LBIND_END_DEFINE_FUNC

LBIND_DEFINE_FUNC(CLabelUI,SetTextColor)
	pThis->SetTextColor(arg[2].toInt());
LBIND_END_DEFINE_FUNC

LBIND_DEFINE_FUNC(CLabelUI,GetTextColor)
	return L.lreturn(pThis->GetTextColor());
LBIND_END_DEFINE_FUNC

LBIND_DEFINE_FUNC(CLabelUI,SetDisabledTextColor)
	pThis->SetDisabledTextColor(arg[2].toInt());
LBIND_END_DEFINE_FUNC

LBIND_DEFINE_FUNC(CLabelUI,GetDisabledTextColor)
	return L.lreturn(pThis->GetDisabledTextColor());
LBIND_END_DEFINE_FUNC

LBIND_DEFINE_FUNC(CLabelUI,SetFont)
	pThis->SetFont(CDuiString(arg[2].toString()));
LBIND_END_DEFINE_FUNC

LBIND_DEFINE_FUNC(CLabelUI,GetFont)
	return L.lreturn(pThis->GetFont());
LBIND_END_DEFINE_FUNC

LBIND_DEFINE_FUNC(CLabelUI,SetTextPadding)
	pThis->SetTextPadding(toRect(arg[2]));
LBIND_END_DEFINE_FUNC

LBIND_DEFINE_FUNC(CLabelUI,GetTextPadding)
	return L.lreturn(toLua(L,pThis->GetTextPadding()));
LBIND_END_DEFINE_FUNC

LBIND_DEFINE_FUNC(CLabelUI,SetShowHtml)
	pThis->SetShowHtml(arg[2].toBool());
LBIND_END_DEFINE_FUNC

LBIND_DEFINE_FUNC(CLabelUI,IsShowHtml)
	return L.lreturn(pThis->IsShowHtml());
LBIND_END_DEFINE_FUNC

}


LBIND_BEGIN_DEFINE_LIB(CLabelUI)
	{"setTextStyle",lbind::SetTextStyle},
	{"textStyle",lbind::GetTextStyle},
	{"setTextColor",lbind::SetTextColor},
	{"textColor",lbind::GetTextColor},
	{"setDisabledTextColor",lbind::SetDisabledTextColor},
	{"disabledTextColor",lbind::GetDisabledTextColor},
	{"setFont",lbind::SetFont},
	{"font",lbind::GetFont},
	{"setTextPadding",lbind::SetTextPadding},
	{"textPadding",lbind::GetTextPadding},
	{"setShowHtml",lbind::SetShowHtml},
	{"isShowHtml",lbind::IsShowHtml},
LBIND_END_DEFINE_LIB





namespace lbind
{
}


LBIND_BEGIN_DEFINE_LIB(CButtonUI)
LBIND_END_DEFINE_LIB



namespace lbind
{
LBIND_DEFINE_FUNC(COptionUI,SetGroup)
	pThis->SetGroup(CDuiString(arg[2].toString()));
LBIND_END_DEFINE_FUNC

LBIND_DEFINE_FUNC(COptionUI,GetGroup)
	return L.lreturn(pThis->GetGroup());
LBIND_END_DEFINE_FUNC

// LBIND_DEFINE_FUNC(COptionUI,SetSelectedTextColor)
// 	pThis->SetSelectedTextColor(arg[2].toInt());
// LBIND_END_DEFINE_FUNC
// 
// LBIND_DEFINE_FUNC(COptionUI,GetSelectedTextColor)
// 	return L.lreturn(pThis->GetSelectedTextColor());
// LBIND_END_DEFINE_FUNC

LBIND_DEFINE_FUNC(COptionUI,Selected)
	pThis->Selected(arg[2].toBool());
LBIND_END_DEFINE_FUNC

LBIND_DEFINE_FUNC(COptionUI,IsSelected)
	return L.lreturn(pThis->IsSelected());
LBIND_END_DEFINE_FUNC

}

LBIND_BEGIN_DEFINE_LIB(COptionUI)
	{"setGroup",lbind::SetGroup},
	{"group",lbind::GetGroup},
// 	{"setSelectedTextColor",lbind::SetSelectedTextColor},
// 	{"selectedTextColor",lbind::GetSelectedTextColor},
	{"selected",lbind::Selected},
	{"isSelected",lbind::IsSelected},

LBIND_END_DEFINE_LIB




namespace lbind
{
LBIND_DEFINE_FUNC(CTextUI,GetLinkContent)
	return L.lreturn(pThis->GetLinkContent(arg[2].toInt()));
LBIND_END_DEFINE_FUNC

}

LBIND_BEGIN_DEFINE_LIB(CTextUI)
	{"linkContent",lbind::GetLinkContent},
LBIND_END_DEFINE_LIB



namespace lbindprogress
{


LBIND_DEFINE_FUNC(CProgressUI,IsHorizontal)
	return L.lreturn(pThis->IsHorizontal());
LBIND_END_DEFINE_FUNC

LBIND_DEFINE_FUNC(CProgressUI,SetType)
	pThis->SetType((CProgressUI::ProgressType)arg[2].toInt());
LBIND_END_DEFINE_FUNC

LBIND_DEFINE_FUNC(CProgressUI,GetMinValue)
	return L.lreturn(pThis->GetMinValue());
LBIND_END_DEFINE_FUNC

LBIND_DEFINE_FUNC(CProgressUI,SetMinValue)
	pThis->SetMinValue(arg[2].toInt());
LBIND_END_DEFINE_FUNC

LBIND_DEFINE_FUNC(CProgressUI,GetMaxValue)
	return L.lreturn(pThis->GetMaxValue());
LBIND_END_DEFINE_FUNC

LBIND_DEFINE_FUNC(CProgressUI,SetMaxValue)
	pThis->SetMaxValue(arg[2].toInt());
LBIND_END_DEFINE_FUNC

LBIND_DEFINE_FUNC(CProgressUI,GetValue)
	return L.lreturn(pThis->GetValue());
LBIND_END_DEFINE_FUNC

LBIND_DEFINE_FUNC(CProgressUI,SetValue)
	pThis->SetValue(arg[2].toInt());
LBIND_END_DEFINE_FUNC

}


LBIND_BEGIN_DEFINE_LIB(CProgressUI)
	{"isHorizontal",lbindprogress::IsHorizontal},
	{"setHorizontal",lbindprogress::SetType},
	{"minValue",lbindprogress::GetMinValue},
	{"setMinValue",lbindprogress::SetMinValue},
	{"maxValue",lbindprogress::GetMaxValue},
	{"setMaxValue",lbindprogress::SetMaxValue},
	{"value",lbindprogress::GetValue},
	{"setValue",lbindprogress::SetValue},
LBIND_END_DEFINE_LIB



namespace lbind
{

LBIND_DEFINE_FUNC(CSliderUI,GetChangeStep)
	return L.lreturn(pThis->GetChangeStep());
LBIND_END_DEFINE_FUNC

LBIND_DEFINE_FUNC(CSliderUI,SetChangeStep)
	pThis->SetChangeStep(arg[2].toInt());
LBIND_END_DEFINE_FUNC


LBIND_DEFINE_FUNC(CSliderUI,SetThumbSize)
	pThis->SetThumbSize(toSize(arg[2]));
LBIND_END_DEFINE_FUNC

LBIND_DEFINE_FUNC(CSliderUI,GetThumbRect)
	L.lreturn(toLua(L,pThis->GetThumbRect()));
LBIND_END_DEFINE_FUNC

LBIND_DEFINE_FUNC(CSliderUI,GetThumbImage)
	return L.lreturn(pThis->GetThumbImage());
LBIND_END_DEFINE_FUNC

LBIND_DEFINE_FUNC(CSliderUI,SetThumbImage)
	pThis->SetThumbImage(CDuiString(arg[2].toString()));
LBIND_END_DEFINE_FUNC

LBIND_DEFINE_FUNC(CSliderUI,GetThumbHotImage)
	return L.lreturn(pThis->GetThumbHotImage());
LBIND_END_DEFINE_FUNC

LBIND_DEFINE_FUNC(CSliderUI,SetThumbHotImage)
	pThis->SetThumbHotImage(CDuiString(arg[2].toString()));
LBIND_END_DEFINE_FUNC

LBIND_DEFINE_FUNC(CSliderUI,GetThumbPushedImage)
	return L.lreturn(pThis->GetThumbPushedImage());
LBIND_END_DEFINE_FUNC

LBIND_DEFINE_FUNC(CSliderUI,SetThumbPushedImage)
	pThis->SetThumbPushedImage(CDuiString(arg[2].toString()));
LBIND_END_DEFINE_FUNC
}

LBIND_BEGIN_DEFINE_LIB(CSliderUI)
	{"changeStep",lbind::GetChangeStep},
	{"setChangeStep",lbind::SetChangeStep},
	{"setThumbSize",lbind::SetThumbSize},
	{"thumbRect",lbind::GetThumbRect},
	{"thumbImage",lbind::GetThumbImage},
	{"setThumbImage",lbind::SetThumbImage},
	{"thumbHotImage",lbind::GetThumbHotImage},
	{"setThumbHotImage",lbind::SetThumbHotImage},
	{"thumbPushedImage",lbind::GetThumbPushedImage},
	{"setThumbPushedImage",lbind::SetThumbPushedImage},
LBIND_END_DEFINE_LIB


namespace lbindedit
{

LBIND_DEFINE_FUNC(CEditUI,GetMaxChar)
	return L.lreturn(pThis->GetMaxChar());
LBIND_END_DEFINE_FUNC

LBIND_DEFINE_FUNC(CEditUI,SetMaxChar)
	pThis->SetMaxChar(arg[2].toInt());
LBIND_END_DEFINE_FUNC

LBIND_DEFINE_FUNC(CEditUI,IsReadOnly)
	return L.lreturn(pThis->IsReadOnly());
LBIND_END_DEFINE_FUNC

LBIND_DEFINE_FUNC(CEditUI,SetReadOnly)
	pThis->SetReadOnly(arg[2].toBool());
LBIND_END_DEFINE_FUNC

LBIND_DEFINE_FUNC(CEditUI,IsPasswordMode)
	return L.lreturn(pThis->IsPasswordMode());
LBIND_END_DEFINE_FUNC

LBIND_DEFINE_FUNC(CEditUI,SetPasswordMode)
	pThis->SetPasswordMode(arg[2].toBool());
LBIND_END_DEFINE_FUNC

LBIND_DEFINE_FUNC(CEditUI,GetPasswordChar)
	return L.lreturn(pThis->GetPasswordChar());
LBIND_END_DEFINE_FUNC

LBIND_DEFINE_FUNC(CEditUI,SetPasswordChar)
	pThis->SetPasswordChar(arg[2].toInt());
LBIND_END_DEFINE_FUNC

LBIND_DEFINE_FUNC(CEditUI,GetNativeEditBkColor)
	return L.lreturn(pThis->GetNativeEditBkColor());
LBIND_END_DEFINE_FUNC

LBIND_DEFINE_FUNC(CEditUI,SetNativeEditBkColor)
	pThis->SetNativeEditBkColor(arg[2].toInt());
LBIND_END_DEFINE_FUNC
}

LBIND_BEGIN_DEFINE_LIB(CEditUI)
	{"maxChar",lbindedit::GetMaxChar},
	{"setMaxChar",lbindedit::SetMaxChar},
	{"isReadOnly",lbindedit::IsReadOnly},
	{"setReadOnly",lbindedit::SetReadOnly},
	{"isPasswordMode",lbindedit::IsPasswordMode},
	{"setPasswordMode",lbindedit::SetPasswordMode},
	{"passwordChar",lbindedit::GetPasswordChar},
	{"nativeEditBkColor",lbindedit::GetNativeEditBkColor},
	{"setNativeEditBkColor",lbindedit::SetNativeEditBkColor},
LBIND_END_DEFINE_LIB

namespace lbind
{

LBIND_DEFINE_FUNC(CScrollBarUI,IsHorizontal)
	return L.lreturn(pThis->IsHorizontal());
LBIND_END_DEFINE_FUNC

LBIND_DEFINE_FUNC(CScrollBarUI,SetHorizontal)
	pThis->SetHorizontal(arg[2].toBool());
LBIND_END_DEFINE_FUNC

LBIND_DEFINE_FUNC(CScrollBarUI,GetScrollRange)
	return L.lreturn(pThis->GetScrollRange());
LBIND_END_DEFINE_FUNC

LBIND_DEFINE_FUNC(CScrollBarUI,SetScrollRange)
	pThis->SetScrollRange(arg[2].toInt());
LBIND_END_DEFINE_FUNC


LBIND_DEFINE_FUNC(CScrollBarUI,GetScrollPos)
	return L.lreturn(pThis->GetScrollPos());
LBIND_END_DEFINE_FUNC

LBIND_DEFINE_FUNC(CScrollBarUI,SetScrollPos)
	pThis->SetScrollPos(arg[2].toInt());
LBIND_END_DEFINE_FUNC

LBIND_DEFINE_FUNC(CScrollBarUI,GetLineSize)
	return L.lreturn(pThis->GetLineSize());
LBIND_END_DEFINE_FUNC

LBIND_DEFINE_FUNC(CScrollBarUI,SetLineSize)
	pThis->SetLineSize(arg[2].toInt());
LBIND_END_DEFINE_FUNC

}



LBIND_BEGIN_DEFINE_LIB(CScrollBarUI)
	{"isHorizontal",lbind::IsHorizontal},
	{"setHorizontal",lbind::SetHorizontal},
	{"scrollRange",lbind::GetScrollRange},
	{"setScrollRange",lbind::SetScrollRange},
	{"scrollPos",lbind::GetScrollPos},
	{"setScrollPos",lbind::SetScrollPos},
	{"lineSize",lbind::GetLineSize},
	{"setLineSize",lbind::SetLineSize},
LBIND_END_DEFINE_LIB