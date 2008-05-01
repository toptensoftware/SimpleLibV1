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
// AtlDrawInfo.h - declaration of AtlDrawInfo

#ifndef __ATLDRAWINFO_H
#define __ATLDRAWINFO_H

namespace Simple
{

struct ATL_DRAWINFO;

// Helpers for normalizing ATL_DRAWINFO 
void SIMPLEAPI AtlMapZoom(ATL_DRAWINFO& di);
void SIMPLEAPI AtlZeroOrigin(ATL_DRAWINFO& di);

}	// namespace Simple

#endif	// __ATLDRAWINFO_H

