//////////////////////////////////////////////////////////////////////
//
// SimpleLib ATL Win Version 1.0
//
// Copyright (C) 1998-2007 Topten Software.  All Rights Reserved
// http://www.toptensoftware.com
//
// This code has been released for use "as is".  Any redistribution or 
// modification however is strictly prohibited.   See the readme.txt file 
// for complete terms and conditions.
//
//////////////////////////////////////////////////////////////////////

#ifndef __SIMPLELIBATLWIN_H
#define __SIMPLELIBATLWIN_H


// Include other SimpleLib headers
#include "SimpleLib.h"
#include "SimpleLibUtils.h"
#include "SimpleLibWin.h"

#ifdef __ATLWIN_H__
#include "SimpleLibAtlWin\atlcontrols.h"
#endif

// Include files from SimpleLibAtlWin
#include "SimpleLibAtlWin\ADXDialog.h"
#include "SimpleLibAtlWin\CheckList.h"
#include "SimpleLibAtlWin\FrameWindow.h"
#include "SimpleLibAtlWin\ListViewUtils.h"
#include "SimpleLibAtlWin\LoadIconEx.h"
#include "SimpleLibAtlWin\MdiWindow.h"
#include "SimpleLibAtlWin\MenuBarApi.h"
#include "SimpleLibAtlWin\MenuIcons.h"
#include "SimpleLibAtlWin\PropertySheet.h"
#include "SimpleLibAtlWin\RegisteredWindow.h"
#include "SimpleLibAtlWin\ResizeablePanel.h"
#include "SimpleLibAtlWin\SimpleToolTip.h"
#include "SimpleLibAtlWin\ToolBarCtrlEx.h"
#include "SimpleLibAtlWin\WindowImplEx.h"


#if _MSC_VER < 1500
	#ifdef _WIN64
		#ifdef _DEBUG
			#ifdef _DLL
			#pragma comment(lib, "SimpleLibAtlWinDD8x64.lib")
			#else
			#pragma comment(lib, "SimpleLibAtlWinDS8x64.lib")
			#endif
		#else
			#ifdef _DLL
			#pragma comment(lib, "SimpleLibAtlWinRD8x64.lib")
			#else
			#pragma comment(lib, "SimpleLibAtlWinRS8x64.lib")
			#endif
		#endif
	#else
		#ifdef _DEBUG
			#ifdef _DLL
			#pragma comment(lib, "SimpleLibAtlWinDD8.lib")
			#else
			#pragma comment(lib, "SimpleLibAtlWinDS8.lib")
			#endif
		#else
			#ifdef _DLL
			#pragma comment(lib, "SimpleLibAtlWinRD8.lib")
			#else
			#pragma comment(lib, "SimpleLibAtlWinRS8.lib")
			#endif
		#endif
	#endif
#else
	#ifdef _WIN64
		#ifdef _DEBUG
			#ifdef _DLL
			#pragma comment(lib, "SimpleLibAtlWinDD9x64.lib")
			#else
			#pragma comment(lib, "SimpleLibAtlWinDS9x64.lib")
			#endif
		#else
			#ifdef _DLL
			#pragma comment(lib, "SimpleLibAtlWinRD9x64.lib")
			#else
			#pragma comment(lib, "SimpleLibAtlWinRS9x64.lib")
			#endif
		#endif
	#else
		#ifdef _DEBUG
			#ifdef _DLL
			#pragma comment(lib, "SimpleLibAtlWinDD9.lib")
			#else
			#pragma comment(lib, "SimpleLibAtlWinDS9.lib")
			#endif
		#else
			#ifdef _DLL
			#pragma comment(lib, "SimpleLibAtlWinRD9.lib")
			#else
			#pragma comment(lib, "SimpleLibAtlWinRS9.lib")
			#endif
		#endif
	#endif
#endif

#endif	// __SIMPLELIBATLWIN_H

