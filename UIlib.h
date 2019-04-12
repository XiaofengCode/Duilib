// Copyright (c) 2010-2011, duilib develop team(www.duilib.com).
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or 
// without modification, are permitted provided that the 
// following conditions are met.
//
// Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// Redistributions in binary form must reproduce the above 
// copyright notice, this list of conditions and the following
// disclaimer in the documentation and/or other materials 
// provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
// CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#if defined(UILIB_EXPORTS)
#if defined(_MSC_VER)
#define UILIB_API __declspec(dllexport)
#else
#define UILIB_API 
#endif
#else
// #if defined(_MSC_VER)
// #define UILIB_API __declspec(dllimport)
// #else
// #define UILIB_API 
// #endif
#define UILIB_API 
#pragma comment(lib,"oledlg.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"comctl32.lib")
//#pragma comment(lib,"Riched20.lib")
#endif

#pragma comment(lib,"gdiplus.lib")

#define UILIB_COMDAT __declspec(selectany)

#if defined _M_IX86
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stddef.h>
#include <richedit.h>
#include <tchar.h>
#include <assert.h>
#include <crtdbg.h>
#include <malloc.h>
#include <comdef.h>
#include <gdiplus.h>

#include "Utils/Utils.h"
#include "Utils/String.h"
#include "Utils/FileHelper.h"
#include "Utils/UIShadow.h"
#include "Utils/UIDelegate.h"
#include "Utils/XUnzip.h"
#include "Utils/GifImage.h"

#include "lua/LuaObj/Luaobj.h"

#include "Core/AttributeManager.h"
#include "Core/UIDefine.h"
#include "Core/UIStringTable.h"
#include "Core/UIManager.h"
#include "Core/UIBase.h"
#include "Core/UIControl.h"
#include "Utils/DuiTrayIcon.h"
#include "Core/UIContainer.h"
#include "Core/UIMarkup.h"
#include "Core/UIDlgBuilder.h"
#include "Core/UIRender.h"
#include "Core/UITimer.h"
#include "Utils/WinImplBase.h"


#include "Layout/UIVerticalLayout.h"
#include "Layout/UIHorizontalLayout.h"
#include "Layout/UITileLayout.h"
#include "Layout/UITabLayout.h"
#include "Layout/UIChildLayout.h"

#include "Control/UIList.h"
#include "Control/UICombo.h"
#include "Control/UIScrollBar.h"
#include "Control/UITreeView.h"

#include "Control/UILabel.h"
#include "Control/UIText.h"
#include "Control/UIEdit.h"
#include "Control/UIGifAnim.h"

#include "Control/UIButton.h"
#include "Control/UIOption.h"
#include "Control/UICheckBox.h"

#include "Control/UIProgress.h"
#include "Control/UIProgressButton.h"
#include "Control/UISlider.h"

#include "Control/UIComboBox.h"
#include "Control/UIRichEdit.h"
#include "Control/UIDateTime.h"

#include "Control/UIActiveX.h"
#include "Control/UIWebBrowser.h"
#include "Control/UIFlash.h"

#include "Control/UIMenu.h"
#include "Layout/UISliderTabLayout.h"
#include "Control/UIIpAddress.h"

#define _STR_HELP(R)  #R  
#define STR_MACRO(R)  _STR_HELP(R)  
#ifdef LINK_DUILIB
#	ifdef DUILIB_DLL
#		define DUILIB_TYPE
#	else
#		define DUILIB_TYPE	"s"
#	endif

#	ifdef _DEBUG
		#define DUILIB_CFG	"d"
#	else
#		define DUILIB_CFG
#	endif

#	ifdef _UNICODE
#		define DUILIB_CODE	"u"
#	else
#		define DUILIB_CODE
#	endif

#	ifdef _WIN64
#		define DUILIB_PLAT	"64"
#	else
#		define DUILIB_PLAT
#	endif

#	if _MSC_VER >= 1910
#		define MSC_VERSION	"2017"
#	elif _MSC_VER >= 1900
#		define MSC_VERSION	"2015"
#	elif _MSC_VER >= 1800
#		define MSC_VERSION	"2013"
#	elif _MSC_VER >= 1700
#		define MSC_VERSION	"2012"
#	elif _MSC_VER >= 1600
#		define MSC_VERSION	"2010"
#	endif

#	if defined(_DLL) && defined(_MT)
#		define RT_LINK_TYPE
#	else
#		define RT_LINK_TYPE	"_MT"
#	endif

#	define DUILIB_NAME	"Duilib" MSC_VERSION "_" DUILIB_TYPE DUILIB_CODE DUILIB_CFG DUILIB_PLAT RT_LINK_TYPE ".lib"

#	pragma message("Link duilib name:" DUILIB_NAME)
#	pragma comment(lib, DUILIB_NAME)

#	define LUALIB_NAME	"lua51_" MSC_VERSION".lib"

#	pragma message("Link lua5.1 name:" LUALIB_NAME)
#	pragma comment(lib, LUALIB_NAME)
	
#endif
#pragma message("MSC version:" STR_MACRO(_MSC_VER))
