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
// StandardError.cpp - implementation of StandardError

#include "stdafx.h"
#include "SimpleLibAtlComBuild.h"

#include "StandardError.h"


namespace Simple
{



HRESULT StandardError(HRESULT hr)
{
	SetErrorInfo(0, NULL);
	return hr;
}




}	// namespace Simple
