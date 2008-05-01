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
// ComError.h - declaration of ComError

#ifndef __COMERROR_H
#define __COMERROR_H

namespace Simple
{

HRESULT SIMPLEAPI SetupComErrorInfo(
					HMODULE hModule,
					HRESULT hr, 
					CLSID clsid,
					LPCOLESTR pszSource, 
					LPCOLESTR pszHelpFile,
					DWORD dwHelpContext,
					va_list args
					);

template <class T, class TSource=T>
class CComError
{
public:
	HRESULT ComError(HRESULT hr, ...)
	{
		// Format description string
		va_list args;
		va_start(args, hr);
		hr=SetupComErrorInfo(_Module.GetModuleInstance(), hr, T::GetObjectCLSID(), TSource::GetErrorSource(), TSource::GetErrorHelpFile(), TSource::GetErrorHelpContext(hr), args);
		va_end(args);
		return hr;
	}

	static LPCOLESTR GetErrorSource() { return NULL; };
	static LPCOLESTR GetErrorHelpFile() { return NULL; };
	static DWORD GetErrorHelpContext(HRESULT hr) { return 0; };
};




}	// namespace Simple

#endif	// __COMERROR_H

