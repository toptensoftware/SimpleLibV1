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
// FormatError.h - declaration of FormatError

#ifndef __FORMATERROR_H
#define __FORMATERROR_H

namespace Simple
{

CUniString FormatError(HRESULT hr, IUnknown* pObject=NULL, REFIID riidCalled=CLSID_NULL);

}	// namespace Simple

#endif	// __FORMATERROR_H

