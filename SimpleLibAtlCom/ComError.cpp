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
// ComError.cpp - implementation of ComError

#include "stdafx.h"
#include "SimpleLibAtlComBuild.h"

#include "ComError.h"
#include "FormatBSTR.h"

namespace Simple
{

HRESULT SetupComErrorInfo(
					HMODULE hModule,
					HRESULT hr, 
					CLSID clsid,
					LPCOLESTR pszSource, 
					LPCOLESTR pszHelpFile,
					DWORD dwHelpContext,
					va_list args
					)
{
USES_CONVERSION;

	// Ignore non-errors
	if (hr==S_OK)
		return hr;

	// If its not a failed code, assume its a string resource id
	CUniString strDescription;
	if (SUCCEEDED(hr))
		{
		// Save string id, convert to a failed hresult
		int iString=hr;
		ASSERT(iString>=1000);
		hr=MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, iString);

		// Load the string
		CComBSTR bstr;
		if (!bstr.LoadString(iString))
			{
			ASSERT(FALSE && "LoadString failed in setting up error info (check string defined)");
			}

		// Format error message
		strDescription=Format(static_cast<const wchar_t*>(bstr), args);
		}
	else
		{
		// Get a description
		strDescription=FormatError(hr);
		}

	// Work out source string
	CComBSTR bstrSource;
	if (!pszSource)
		{
		TCHAR szFileName[MAX_PATH];
		GetModuleFileName(hModule, szFileName, MAX_PATH);

		CComPtr<ITypeLib> spTypeLib;
		LoadTypeLibEx(T2COLE(szFileName), REGKIND_NONE, &spTypeLib);

		// Got a type library
		if (spTypeLib)
			{
			// Get name of class
			CComPtr<ITypeInfo> spTypeInfo;
			spTypeLib->GetTypeInfoOfGuid(clsid, &spTypeInfo);
			if (spTypeInfo)
				{
				CComBSTR bstrName;
				CComBSTR bstrDescription;
				if (SUCCEEDED(spTypeInfo->GetDocumentation(MEMBERID_NIL, &bstrName, &bstrDescription, NULL, NULL)))
					{
					if (!IsEmptyString(bstrDescription))
						bstrSource=bstrDescription;
					else
						bstrSource=bstrName;
					}
				}
			}

		if (IsEmptyString(bstrSource))
			{
			// No type lib, use module file name
			bstrSource=szFileName;
			}

		if (!IsEmptyString(bstrSource))
			pszSource=bstrSource;
		}

	// Create error info
	CComPtr<ICreateErrorInfo> spCEI;
	if (FAILED(CreateErrorInfo(&spCEI)))
		return hr;

	// Setup error info
	spCEI->SetGUID(clsid);
	if (!IsEmptyString(pszSource)) 
		spCEI->SetSource(const_cast<LPOLESTR>(pszSource));
	if (!IsEmptyString(strDescription)) 
		spCEI->SetDescription(strDescription.GetBuffer(0));
	if (!IsEmptyString(pszHelpFile))
		{
		spCEI->SetHelpFile(const_cast<LPOLESTR>(pszHelpFile));
		spCEI->SetHelpContext(dwHelpContext);
		}

	// Setup error info...
	SetErrorInfo(0, CComQIPtr<IErrorInfo>(spCEI));
	
	// Done
	return hr;
}






}	// namespace Simple
