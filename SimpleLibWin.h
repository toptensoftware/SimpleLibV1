//////////////////////////////////////////////////////////////////////
//
// SimpleLib Win Version 1.0
//
// Copyright (C) 1998-2007 Topten Software.  All Rights Reserved
// http://www.toptensoftware.com
//
// This code has been released for use "as is".  Any redistribution or 
// modification however is strictly prohibited.   See the readme.txt file 
// for complete terms and conditions.
//
//////////////////////////////////////////////////////////////////////

#ifndef __SIMPLELIBWIN_H
#define __SIMPLELIBWIN_H

#include <Objidl.h>
#include <Shlobj.h>
#include <windows.h>

// Include other SimpleLib headers
#include "SimpleLib.h"
#include "SimpleLibUtils.h"

// Include files from SimpleLibWin
#include "SimpleLibWin\SimpleLibWinMacros.h"
#include "SimpleLibWin\AutoSizeControl.h"
#include "SimpleLibWin\BufferedPaintDC.h"
#include "SimpleLibWin\CallbackTimer.h"
#include "SimpleLibWin\ClipDC.h"
#include "SimpleLibWin\CommandState.h"
#include "SimpleLibWin\CopyStream.h"
#include "SimpleLibWin\CreateFontEasy.h"
#include "SimpleLibWin\CryptorStream.h"
#include "SimpleLibWin\ExpandVariables.h"
#include "SimpleLibWin\FadeWindow.h"
#include "SimpleLibWin\FileUtils.h"
#include "SimpleLibWin\FindFiles.h"
#include "SimpleLibWin\FormatError.h"
#include "SimpleLibWin\FormatUserNumber.h"
#include "SimpleLibWin\LoadString.h"
#include "SimpleLibWin\MessageBox.h"
#include "SimpleLibWin\DataExchange.h"
#include "SimpleLibWin\DocView.h"
#include "SimpleLibWin\DoWebRequest.h"
#include "SimpleLibWin\DragDetectEx.h"
#include "SimpleLibWin\FileStream.h"
#include "SimpleLibWin\GdiMisc.h"
#include "SimpleLibWin\GdiMiscEx.h"
#include "SimpleLibWin\GdiPlusLoader.h"
#include "SimpleLibWin\GdiText.h"
#include "SimpleLibWin\GeoOps.h"
#include "SimpleLibWin\IsInputMessage.h"
#include "SimpleLibWin\LoadSaveTextStream.h"
#include "SimpleLibWin\MessageLoop.h"
#include "SimpleLibWin\MRUFileList.h"
#include "SimpleLibWin\ModuleCRC.h"
#include "SimpleLibWin\MouseTracker.h"
#include "SimpleLibWin\NotifyIcon.h"
#include "SimpleLibWin\PostableObject.h"
#include "SimpleLibWin\ProfileFileBinary.h"
#include "SimpleLibWin\ProfileSettings.h"
#include "SimpleLibWin\ProfileStream.h"  
#include "SimpleLibWin\ReflectMessages.h"
#include "SimpleLibWin\RegistryUtils.h"
#include "SimpleLibWin\ResourceContentProvider.h"
#include "SimpleLibWin\RegistryStream.h"
#include "SimpleLibWin\RunModalLoop.h"
#include "SimpleLibWin\SaveWindowPlacement.h"
#include "SimpleLibWin\SelectObject.h"
#include "SimpleLibWin\SetLayeredWindowAttributesEx.h"
#include "SimpleLibWin\ShellUtils.h"
#include "SimpleLibWin\SimpleDdeClient.h"
#include "SimpleLibWin\SimpleFileDialog.h"
#include "SimpleLibWin\SimpleMemoryStream.h"
#include "SimpleLibWin\Splitter.h"
#include "SimpleLibWin\StreamHelpers.h"
#include "SimpleLibWin\StreamImpl.h"
#include "SimpleLibWin\SubStream.h"
#include "SimpleLibWin\ThemeActivate.h"
#include "SimpleLibWin\ThemedDrawing.h"
#include "SimpleLibWin\Threading.h"
#include "SimpleLibWin\TimerVector.h"
#include "SimpleLibWin\TranslateColor.h"
#include "SimpleLibWin\VersionInfo.h"

#if _MSC_VER < 1500
	#ifdef _WIN64
		#ifdef _DEBUG
			#ifdef _DLL
			#pragma comment(lib, "SimpleLibWinDD8x64.lib")
			#else
			#pragma comment(lib, "SimpleLibWinDS8x64.lib")
			#endif
		#else
			#ifdef _DLL
			#pragma comment(lib, "SimpleLibWinRD8x64.lib")
			#else
			#pragma comment(lib, "SimpleLibWinRS8x64.lib")
			#endif
		#endif
	#else
		#ifdef _DEBUG
			#ifdef _DLL
			#pragma comment(lib, "SimpleLibWinDD8.lib")
			#else
			#pragma comment(lib, "SimpleLibWinDS8.lib")
			#endif
		#else
			#ifdef _DLL
			#pragma comment(lib, "SimpleLibWinRD8.lib")
			#else
			#pragma comment(lib, "SimpleLibWinRS8.lib")
			#endif
		#endif
	#endif
#else
	#ifdef _WIN64
		#ifdef _DEBUG
			#ifdef _DLL
			#pragma comment(lib, "SimpleLibWinDD9x64.lib")
			#else
			#pragma comment(lib, "SimpleLibWinDS9x64.lib")
			#endif
		#else
			#ifdef _DLL
			#pragma comment(lib, "SimpleLibWinRD9x64.lib")
			#else
			#pragma comment(lib, "SimpleLibWinRS9x64.lib")
			#endif
		#endif
	#else
		#ifdef _DEBUG
			#ifdef _DLL
			#pragma comment(lib, "SimpleLibWinDD9.lib")
			#else
			#pragma comment(lib, "SimpleLibWinDS9.lib")
			#endif
		#else
			#ifdef _DLL
			#pragma comment(lib, "SimpleLibWinRD9.lib")
			#else
			#pragma comment(lib, "SimpleLibWinRS9.lib")
			#endif
		#endif
	#endif
#endif

#endif	// __SIMPLELIBWIN_H

