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

//////////////////////////////////////////////////////////////////////////
// FormatError.cpp - implementation of FormatError

#include "stdafx.h"
#include "SimpleLibWinBuild.h"

#include "FormatError.h"

namespace Simple
{

CUniString FormatError(HRESULT hr, IUnknown* pObject, REFIID riidCalled)
{
	// Interface specific error code?
	if (HRESULT_FACILITY(hr)==FACILITY_ITF)
		{
		// Query for ISupportErrorInfo
		CAutoPtr<ISupportErrorInfo, SRefCounted> spSEI;
		if (pObject)
			pObject->QueryInterface(IID_ISupportErrorInfo, (void**)&spSEI);

		// Check if interface supports error info
		if (!pObject || (spSEI && spSEI->InterfaceSupportsErrorInfo(riidCalled)==S_OK))
			{
			// Get error info
			CAutoPtr<IErrorInfo, SRefCounted> spErrorInfo;
			GetErrorInfo(0, &spErrorInfo);

			if (spErrorInfo)
				{
				BSTR bstr=NULL;
				spErrorInfo->GetDescription(&bstr);
				CUniString str(bstr);
				SysFreeString(bstr);
				return str;
				}
			}
		}

	// Replace storage facility with native NT error codes as these don't 
	// have replacement args (eg: %1)
	if (HRESULT_FACILITY(hr)==FACILITY_STORAGE && HRESULT_CODE(hr)<0xF0)
		{
		hr=HRESULT_CODE(hr);
		}

	// Format system error message...
	wchar_t* psz;
	if (!::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS, NULL, 
					hr, LOCALE_USER_DEFAULT, (wchar_t*)&psz, 0, NULL))
		{
		return Format(L"Unknown error (0x%.8x)", hr);
		}
	else
		{
		CUniString str=TrimString(psz);
		LocalFree(psz);
		return str;
		}
}

}	// namespace Simple

