//////////////////////////////////////////////////////////////////////
//
// SimpleLib ATL Win Version 1.0
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
// LoadIconEx.h - declaration of LoadIconEx

#ifndef __LOADICONEX_H
#define __LOADICONEX_H

namespace Simple
{

HICON LoadIconEx(HINSTANCE hInst, LPCTSTR pszRes, bool bLarge);
HICON LoadIconEx(int iIcon, bool bLarge);


}	// namespace Simple

#endif	// __LOADICONEX_H

