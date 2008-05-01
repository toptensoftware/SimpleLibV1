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
// GetControlIcon.h - declaration of GetControlIcon

#ifndef __GETCONTROLICON_H
#define __GETCONTROLICON_H

namespace Simple
{


HBITMAP SIMPLEAPI GetControlImage(LPCOLESTR pszCLSID, int cx, int cy);
HICON SIMPLEAPI GetControlIcon(LPCOLESTR pszCLSID, int cx, int cy);

}	// namespace Simple

#endif	// __GETCONTROLICON_H

