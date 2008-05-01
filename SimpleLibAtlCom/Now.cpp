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
// Now.cpp - implementation of Now

#include "stdafx.h"
#include "SimpleLibAtlComBuild.h"

#include "Now.h"


namespace Simple
{


DATE SIMPLEAPI Now()
{
	SYSTEMTIME st;
	GetLocalTime(&st);

	DATE d;
	SystemTimeToVariantTime(&st, &d);
	return d;
}

DATE SIMPLEAPI NowGMT()
{
	SYSTEMTIME st;
	GetSystemTime(&st);

	DATE d;
	SystemTimeToVariantTime(&st, &d);
	return d;
}

VARIANT SIMPLEAPI NowVariant()
{
	VARIANT v;
	V_VT(&v)=VT_DATE;
	V_DATE(&v)=Now();
	return v;
}

VARIANT SIMPLEAPI NowGMTVariant()
{
	VARIANT v;
	V_VT(&v)=VT_DATE;
	V_DATE(&v)=NowGMT();
	return v;
}


VARIANT SIMPLEAPI DateAsVariant(DATE d)
{
	VARIANT v;
	V_VT(&v)=VT_DATE;
	V_DATE(&v)=d;
	return v;
}



}	// namespace Simple
