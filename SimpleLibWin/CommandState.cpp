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
// CommandState.cpp - implementation of CommandState

#include "stdafx.h"
#include "SimpleLibWinBuild.h"

#include "CommandState.h"

namespace Simple
{

static HMENU g_hMenu=NULL;
static int g_iIndex=-1;
static UINT g_iID=0;

HMENU SIMPLEAPI CommandStateGetMenu()
{
	return g_hMenu;
}

int SIMPLEAPI CommandStateGetMenuIndex()
{
	return g_iIndex;
}

void SIMPLEAPI CommandStateUpdateMenu(HWND hWnd, HMENU hMenu)
{
	g_hMenu=hMenu;
	for (g_iIndex=0; g_iIndex<GetMenuItemCount(hMenu); g_iIndex++)
		{
		UINT nID=GetMenuItemID(hMenu, g_iIndex);
		if (nID!=0xFFFFFFFF && nID<0xF000)
			{
			g_iID=nID;
			LPARAM lProbe=0;
			UINT nState=(UINT)SendMessage(hWnd, WM_COMMANDSTATE, nID, (LPARAM)&lProbe);

			if (nState & CSF_IGNORE)
				continue;

			CheckMenuItem(hMenu, g_iIndex, MF_BYPOSITION| ((nState & CSF_CHECKED) ? MF_CHECKED : 0) );
			EnableMenuItem(hMenu, g_iIndex, MF_BYPOSITION| ((nState & CSF_DISABLED) ? MF_GRAYED : MF_ENABLED) );
			}
		}
	g_hMenu=NULL;
	g_iIndex=-1;
	g_iID=0;
}

void SIMPLEAPI CommandStateSetMenuText(const wchar_t* psz)
{
	if (!g_hMenu)
		return;

	ModifyMenu(g_hMenu, g_iIndex, MF_BYPOSITION|MF_STRING, g_iID, psz);
}


void SIMPLEAPI CommandStateUpdateToolBar(HWND hWnd, HWND hWndToolBar, int iFirst, int iLast)
{
	if (iLast<0)
		iLast=(int)SendMessage(hWndToolBar, TB_BUTTONCOUNT, 0, 0)-1;;
	for (int i=iFirst; i<=iLast; i++)
		{
		TBBUTTON button;
		SendMessage(hWndToolBar, TB_GETBUTTON, i, (LPARAM)&button);

		if (!button.idCommand)
			continue;

		LPARAM lProbe=0;
		UINT nState=(UINT)SendMessage(hWnd, WM_COMMANDSTATE, button.idCommand, (LPARAM)&lProbe);

		if (nState & CSF_IGNORE)
			continue;

		BYTE bNewState=button.fsState & ~(TBSTATE_ENABLED|TBSTATE_CHECKED);
		BYTE bNewStyle=button.fsStyle;
		if (nState & CSF_CHECKED)
			{
			bNewState |= TBSTATE_CHECKED;
			bNewStyle |= TBSTYLE_CHECK;
			}
		if ((nState & CSF_DISABLED)==0)
			{
			bNewState |= TBSTATE_ENABLED;
			}

		if (bNewState!=button.fsState || bNewStyle!=button.fsStyle)
			{
			TBBUTTONINFO info;
			info.cbSize=sizeof(info);
			info.dwMask=TBIF_STYLE|TBIF_STATE;
			info.fsStyle=bNewStyle;
			info.fsState=bNewState;
			SendMessage(hWndToolBar, TB_SETBUTTONINFO, button.idCommand, (LPARAM)&info);
			}
		
		}

}

}	// namespace Simple
