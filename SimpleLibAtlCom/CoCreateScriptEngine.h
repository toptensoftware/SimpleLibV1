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

//////////////////////////////////////////////////////////////////////////
// CoCreateScriptEngine.h - declaration of CoCreateScriptEngine

#ifndef __COCREATESCRIPTENGINE_H
#define __COCREATESCRIPTENGINE_H

namespace Simple
{

// Optimized version of CoCreateInstance for creating script engines.  Bypasses installed
// McAfee scanners to save huge memory footprint.

HRESULT CoCreateInstanceDLL(LPCOLESTR pszDLL, CLSID clsid, REFIID riid, void** ppvObject);
HRESULT CoCreateScriptEngine(LPCOLESTR pszProgID, REFIID riid, void** pVal);


}	// namespace Simple

#endif	// __COCREATESCRIPTENGINE_H

