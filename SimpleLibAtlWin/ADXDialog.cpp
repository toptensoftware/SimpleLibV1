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

#include "stdafx.h"
#include "SimpleLibAtlWinBuild.h"

#include "ADXDialog.h"

namespace Simple
{

void ADX_Text(CADXExchange* pDX, int nIDC, CComBSTR& str)
{
	// Get the control
	HWND hWnd=pDX->PrepareControl(nIDC);

	if (pDX->m_bSave)
		{
		str.Empty();
		CWindow(hWnd).GetWindowText(str.m_str);
		}
	else
		{
		SetWindowText(hWnd, str);
		}
}

}

