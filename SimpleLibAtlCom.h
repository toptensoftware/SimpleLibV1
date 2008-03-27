//////////////////////////////////////////////////////////////////////
//
// SimpleLib ATL COM Version 1.0
//
// Copyright (C) 1998-2007 Topten Software.  All Rights Reserved
// http://www.toptensoftware.com
//
// This code has been released for use "as is".  Any redistribution or 
// modification however is strictly prohibited.   See the readme.txt file 
// for complete terms and conditions.
//
//////////////////////////////////////////////////////////////////////

#ifndef __SIMPLELIBATLCOM_H
#define __SIMPLELIBATLCOM_H

// Include other SimpleLib headers
#include "SimpleLib.h"
#include "SimpleLibUtils.h"
#include "SimpleLibAtlWin.h"

#include "SimpleLibAtlCom\AreFontsEqual.h"
#include "SimpleLibAtlCom\AtlDrawInfo.h"
#include "SimpleLibAtlCom\BorderStyle.h"
#include "SimpleLibAtlCom\CoCreateScriptEngine.h"
#include "SimpleLibAtlCom\ComControlEx.h"
#include "SimpleLibAtlCom\ComError.h"
#include "SimpleLibAtlCom\ComObjectOwned.h"
#include "SimpleLibAtlCom\ComObjectSink.h"
#include "SimpleLibAtlCom\ComPtrVector.h"
#include "SimpleLibAtlCom\DispatchProperty.h"
#include "SimpleLibAtlCom\EnumVariantImpl.h"
#include "SimpleLibAtlCom\FontHolder.h"
#include "SimpleLibAtlCom\FormatBSTR.h"
#include "SimpleLibAtlCom\GetControlIcon.h"
#include "SimpleLibAtlCom\IsControlInDesignMode.h"
#include "SimpleLibAtlCom\Now.h"
#include "SimpleLibAtlCom\ObjectIdentity.h"
#include "SimpleLibAtlCom\ObjectWithNamedValues.h"
#include "SimpleLibAtlCom\ObjectWithWeakOwner.h"
#include "SimpleLibAtlCom\OleCommandTargetImpl.h"
#include "SimpleLibAtlCom\OleKeyMouse.h"
#include "SimpleLibAtlCom\PerThreadSingleton.h"
#include "SimpleLibAtlCom\PropertyBagOnXML.h"
#include "SimpleLibAtlCom\PropertyNotifyHandler.h"
#include "SimpleLibAtlCom\SafeArray.h"
#include "SimpleLibAtlCom\SimpleDataObject.h"
#include "SimpleLibAtlCom\SimpleDragDrop.h"
#include "SimpleLibAtlCom\SimpleEventSink.h"
#include "SimpleLibAtlCom\SimpleEventSinkBase.h"
#include "SimpleLibAtlCom\SimplePropertyPageImpl.h"
#include "SimpleLibAtlCom\SimpleSink.h"
#include "SimpleLibAtlCom\SpecifyPropertyPagesImpl2.h"
#include "SimpleLibAtlCom\StandardError.h"
#include "SimpleLibAtlCom\SubclassedControl.h"
#include "SimpleLibAtlCom\TypeInfoHelpers.h"
#include "SimpleLibAtlCom\VariantGrid.h"
#include "SimpleLibAtlCom\VariantParams.h"
#include "SimpleLibAtlCom\XMLArchive.h"

#if _MSC_VER < 1500
	#ifdef _WIN64
		#ifdef _DEBUG
			#ifdef _DLL
			#pragma comment(lib, "SimpleLibAtlComDD8x64.lib")
			#else
			#pragma comment(lib, "SimpleLibAtlComDS8x64.lib")
			#endif
		#else
			#ifdef _DLL
			#pragma comment(lib, "SimpleLibAtlComRD8x64.lib")
			#else
			#pragma comment(lib, "SimpleLibAtlComRS8x64.lib")
			#endif
		#endif
	#else
		#ifdef _DEBUG
			#ifdef _DLL
			#pragma comment(lib, "SimpleLibAtlComDD8.lib")
			#else
			#pragma comment(lib, "SimpleLibAtlComDS8.lib")
			#endif
		#else
			#ifdef _DLL
			#pragma comment(lib, "SimpleLibAtlComRD8.lib")
			#else
			#pragma comment(lib, "SimpleLibAtlComRS8.lib")
			#endif
		#endif
	#endif
#else
	#ifdef _WIN64
		#ifdef _DEBUG
			#ifdef _DLL
			#pragma comment(lib, "SimpleLibAtlComDD9x64.lib")
			#else
			#pragma comment(lib, "SimpleLibAtlComDS9x64.lib")
			#endif
		#else
			#ifdef _DLL
			#pragma comment(lib, "SimpleLibAtlComRD9x64.lib")
			#else
			#pragma comment(lib, "SimpleLibAtlComRS9x64.lib")
			#endif
		#endif
	#else
		#ifdef _DEBUG
			#ifdef _DLL
			#pragma comment(lib, "SimpleLibAtlComDD9.lib")
			#else
			#pragma comment(lib, "SimpleLibAtlComDS9.lib")
			#endif
		#else
			#ifdef _DLL
			#pragma comment(lib, "SimpleLibAtlComRD9.lib")
			#else
			#pragma comment(lib, "SimpleLibAtlComRS9.lib")
			#endif
		#endif
	#endif
#endif

#endif	// __SIMPLELIBATLCOM_H

