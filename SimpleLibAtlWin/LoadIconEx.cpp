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
// FrameWindow.cpp - implementation of CBarFrameWindowImpl class

#include "stdafx.h"
#include "SimpleLibAtlWinBuild.h"

#include "FrameWindow.h"

namespace Simple
{

HICON SIMPLEAPI LoadIconEx(HINSTANCE hInst, LPCTSTR pszRes, bool bLarge)
{
	return (HICON)LoadImage(hInst, pszRes, IMAGE_ICON, 
					GetSystemMetrics(bLarge ? SM_CXICON : SM_CXSMICON),
					GetSystemMetrics(bLarge ? SM_CYICON : SM_CYSMICON), LR_DEFAULTCOLOR);

}


HICON SIMPLEAPI LoadIconEx(int iIcon, bool bLarge)
{
	return LoadIconEx(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(iIcon), bLarge);
}


}	// namespace Simple
