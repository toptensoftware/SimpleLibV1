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
// AtlDrawInfo.cpp - implementation of AtlDrawInfo

#include "stdafx.h"
#include "SimpleLibAtlComBuild.h"


void AtlMapZoom(ATL_DRAWINFO& di)
{
	// Quit if not zoomed
	if (!di.bZoomed)
		return;

	RECT& rc = *(RECT*)di.prcBounds;
	
	// Setup mapping mode...
	SetMapMode(di.hdcDraw, MM_ANISOTROPIC);
	SetViewportExtEx(di.hdcDraw, di.ZoomNum.cx, di.ZoomNum.cy, NULL);
	SetWindowExtEx(di.hdcDraw, di.ZoomDen.cx, di.ZoomDen.cy, NULL);
	SetWindowOrgEx(di.hdcDraw, rc.left, rc.top, NULL);
	SetViewportOrgEx(di.hdcDraw, rc.left, rc.top, NULL);

	// Scale bounds...
	rc.right=rc.left + (rc.right-rc.left) * di.ZoomDen.cx / di.ZoomNum.cx;
	rc.bottom=rc.top + (rc.bottom-rc.top) * di.ZoomDen.cy / di.ZoomNum.cy;
}

void AtlZeroOrigin(ATL_DRAWINFO& di)
{
	RECT& rc = *(RECT*)di.prcBounds;

	// Quit if already zero origin
	if (rc.left==0 && rc.top==0)
		return;

	OffsetWindowOrgEx(di.hdcDraw, -rc.left, -rc.top, NULL);
	OffsetRect(&rc, -rc.left, -rc.top);
}

