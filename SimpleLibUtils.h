
//////////////////////////////////////////////////////////////////////
//
// SimpleLib Utils Version 1.0
//
// Copyright (C) 1998-2007 Topten Software.  All Rights Reserved
// http://www.toptensoftware.com
//
// This code has been released for use "as is".  Any redistribution or
// modification however is strictly prohibited.   See the readme.txt file
// for complete terms and conditions.
//
//////////////////////////////////////////////////////////////////////

#ifndef __SIMPLELIBUTILS_H
#define __SIMPLELIBUTILS_H

// Include other SimpleLib headers
#include "SimpleLib.h"

// Include files from SimpleLibUtils
#include "SimpleLibUtils/Platform.h"
#include "SimpleLibUtils/Result.h"
#include "SimpleLibUtils/CalcCRC.h"
#include "SimpleLibUtils/CommandLineParser.h"
#include "SimpleLibUtils/Cryptor.h"
#include "SimpleLibUtils/FormatBinary.h"
#include "SimpleLibUtils/File.h"
#include "SimpleLibUtils/IncrementString.h"
#include "SimpleLibUtils/ParseUtils.h"
#include "SimpleLibUtils/ParseHtmlColor.h"
#include "SimpleLibUtils/PathLibrary.h"
#include "SimpleLibUtils/ProfileFile.h"
#include "SimpleLibUtils/LoadSaveTextFile.h"
#include "SimpleLibUtils/ResNode.h"
#include "SimpleLibUtils/ResParser.h"
#include "SimpleLibUtils/ResBinary.h"
#include "SimpleLibUtils/ResTemplate.h"
#include "SimpleLibUtils/Sha1.h"
#include "SimpleLibUtils/SmartStringCompare.h"
#include "SimpleLibUtils/SplitCommandLine.h"
#include "SimpleLibUtils/SplitString.h"
#include "SimpleLibUtils/StringReplace.h"
#include "SimpleLibUtils/StringSearch.h"
#include "SimpleLibUtils/Tokenizer.h"
#include "SimpleLibUtils/TrimString.h"
#include "SimpleLibUtils/Wildcard.h"
#include "SimpleLibUtils/GiantClass.h"

#ifdef _MSC_VER
#if _MSC_VER < 1500
	#ifdef _WIN64
		#ifdef _DEBUG
			#ifdef _DLL
			#pragma comment(lib, "SimpleLibUtilsDD8x64.lib")
			#else
			#pragma comment(lib, "SimpleLibUtilsDS8x64.lib")
			#endif
		#else
			#ifdef _DLL
			#pragma comment(lib, "SimpleLibUtilsRD8x64.lib")
			#else
			#pragma comment(lib, "SimpleLibUtilsRS8x64.lib")
			#endif
		#endif
	#else
		#ifdef _DEBUG
			#ifdef _DLL
			#pragma comment(lib, "SimpleLibUtilsDD8.lib")
			#else
			#pragma comment(lib, "SimpleLibUtilsDS8.lib")
			#endif
		#else
			#ifdef _DLL
			#pragma comment(lib, "SimpleLibUtilsRD8.lib")
			#else
			#pragma comment(lib, "SimpleLibUtilsRS8.lib")
			#endif
		#endif
	#endif
#else
	#ifdef _WIN64
		#ifdef _DEBUG
			#ifdef _DLL
			#pragma comment(lib, "SimpleLibUtilsDD9x64.lib")
			#else
			#pragma comment(lib, "SimpleLibUtilsDS9x64.lib")
			#endif
		#else
			#ifdef _DLL
			#pragma comment(lib, "SimpleLibUtilsRD9x64.lib")
			#else
			#pragma comment(lib, "SimpleLibUtilsRS9x64.lib")
			#endif
		#endif
	#else
		#ifdef _DEBUG
			#ifdef _DLL
			#pragma comment(lib, "SimpleLibUtilsDD9.lib")
			#else
			#pragma comment(lib, "SimpleLibUtilsDS9.lib")
			#endif
		#else
			#ifdef _DLL
			#pragma comment(lib, "SimpleLibUtilsRD9.lib")
			#else
			#pragma comment(lib, "SimpleLibUtilsRS9.lib")
			#endif
		#endif
	#endif
#endif
#endif

#endif	// __SIMPLELIBUTILS_H

