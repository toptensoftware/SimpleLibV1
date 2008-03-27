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
// CoCreateScriptEngine.cpp - implementation of CoCreateScriptEngine

#include "stdafx.h"
#include "SimpleLibAtlComBuild.h"

#include "CoCreateScriptEngine.h"

namespace Simple
{

typedef HRESULT (STDMETHODCALLTYPE *PFNDLLGETCLASSOBJECT)(REFCLSID rclsid, REFIID riid, LPVOID* ppv);

HRESULT CoCreateInstanceDLL(LPCOLESTR pszDLL, CLSID clsid, REFIID riid, void** ppvObject)
{
USES_CONVERSION;

	// Load the module
	HMODULE hModule=LoadLibrary(OLE2CT(pszDLL));
	if (!hModule)
		return HRESULT_FROM_WIN32(GetLastError());

	// Get address of DllGetClassObject
	PFNDLLGETCLASSOBJECT pfnDllGetClassObject=(PFNDLLGETCLASSOBJECT)GetProcAddress(hModule, "DllGetClassObject");
	if (!pfnDllGetClassObject)
		return REGDB_E_CLASSNOTREG;

	// Get class factory
	CComPtr<IClassFactory> spClassFactory;
	RETURNIFFAILED(pfnDllGetClassObject(clsid, IID_IClassFactory, (void**)&spClassFactory));

	// Create it
	return spClassFactory->CreateInstance(NULL, riid, ppvObject);
}

static CLSID CLSID_VBScript={0xB54F3741, 0x5B07, 0x11cf, 0xA4, 0xB0, 0x00, 0xAA, 0x00, 0x4A, 0x55, 0xE8};
static CLSID CLSID_JScript={0xF414C260, 0x6AC0, 0x11CF, 0xB6, 0xD1, 0x00, 0xAA, 0x00, 0xBB, 0xBB, 0x58};

#include <activscp.h>

HRESULT CoCreateScriptEngine(LPCOLESTR pszProgID, REFIID riid, void** pVal)
{
	if (IsEqualStringI(pszProgID, L"VBScript"))
		{
		if (CoCreateInstanceDLL(SimplePathAppend(SlxGetSystemDirectory(), L"VBScript.dll"), CLSID_VBScript, riid, pVal)==S_OK)
			return S_OK;
		}

	if (IsEqualStringI(pszProgID, L"JScript"))
		{
		if (CoCreateInstanceDLL(SimplePathAppend(SlxGetSystemDirectory(), L"JScript.dll"), CLSID_VBScript, riid, pVal)==S_OK)
			return S_OK;
		}

	CComPtr<IUnknown> sp;
	RETURNIFFAILED(sp.CoCreateInstance(pszProgID));
	return sp->QueryInterface(riid, pVal);
}

}