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
//
//
// DirectUI - UI Library
//
// Written by Bjarke Viksoe (bjarke@viksoe.dk)
// Copyright (c) 2006-2007 Bjarke Viksoe.
//
// This code may be used in compiled form in any way you desire. These
// source files may be redistributed by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage to you or your
// computer whatsoever. It's free, so don't hassle me about it.
//
// Beware of bugs.
//
//


#include "stdafx.h"
#include "UIlib.h"


#define GIT_MODULE_NAME "Duilib"

#ifndef _AUTO_INFO_H_
#define _AUTO_INFO_H_

#define AUTO_INFO_COMMIT	1
#define AUTO_INFO_VERSION	2
#define AUTO_INFO_BRANCH	3
#define AUTO_INFO_MODULE	4
typedef struct
{
	DWORD dwType; // AUTO_INFO_xxxx
	CHAR szValue[64];
	BYTE byUnused[28];
}AUTO_INFO_ITEM;

typedef struct
{
	CHAR byAutoVersion[16];
	BYTE byCount;
	BYTE byUnused[3];
}AUTO_INFO_HEAD;

#endif // _AUTO_INFO_H_

struct
{
	AUTO_INFO_HEAD head;
	AUTO_INFO_ITEM item[3];
}g_AutoInfo = {
	{
		"AUTO_VERSION_V1",
		3,
		""
	},
	{
		{
			AUTO_INFO_MODULE,
			GIT_MODULE_NAME,
		},
		{
			AUTO_INFO_COMMIT,
#include "gitcommit.log"
			,
		},
		{
			AUTO_INFO_BRANCH,
#include "gitbranch.log"
			,
		}
	}
};

BOOL APIENTRY DllMain(HANDLE hModule, DWORD  dwReason, LPVOID /*lpReserved*/)
{
    switch( dwReason ) {
   case DLL_PROCESS_ATTACH:
   case DLL_THREAD_ATTACH:
   case DLL_THREAD_DETACH:
   case DLL_PROCESS_DETACH:
       ::DisableThreadLibraryCalls((HMODULE)hModule);
       break;
    }
    return TRUE;
}

