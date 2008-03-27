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
// Now.h - declaration of Now

#ifndef __NOW_H
#define __NOW_H

namespace Simple
{

DATE Now();
VARIANT NowVariant();
DATE NowGMT();
VARIANT NowGMTVariant();
VARIANT DateAsVariant(DATE d);


}	// namespace Simple

#endif	// __NOW_H

