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
// DragDetectEx.cpp - implementation of DragDetectEx

#include "stdafx.h"
#include "SimpleLibWinBuild.h"

#include "DragDetectEx.h"


namespace Simple
{

/////////////////////////////////////////////////////////////////////////////
// DragDetectEx

BOOL SIMPLEAPI DragDetectEx(HWND hWnd, POINT pt, DWORD dwFlags)
{
	// Work out what message to cancel drag
	UINT uMouseUpMessage=0;
	if (GetKeyState(VK_MBUTTON)<0) uMouseUpMessage=WM_MBUTTONUP;
	if (GetKeyState(VK_RBUTTON)<0) uMouseUpMessage=WM_RBUTTONUP;
	if (GetKeyState(VK_LBUTTON)<0) uMouseUpMessage=WM_LBUTTONUP;
	if (uMouseUpMessage==0)
		return FALSE;

	// Capture mouse
	SetCapture(hWnd);

	// Get delay amount
	DWORD dwDragDelay=GetProfileIntA("windows", "DragDelay", 200);

	ClientToScreen(hWnd, &pt);

	// Work out rectangle must drag outside of
	RECT rc;
	rc.left=pt.x-GetSystemMetrics(SM_CXDRAG);
	rc.right=pt.x+GetSystemMetrics(SM_CXDRAG);
	rc.top=pt.y-GetSystemMetrics(SM_CYDRAG);
	rc.bottom=pt.y+GetSystemMetrics(SM_CYDRAG);

	// Get tick start
	DWORD dwTickStart=GetTickCount();

	// Message loop
	while (TRUE )
		{
		// More than 300 millisecond, start drag...
		if ((dwFlags & DDXF_NOTIMER)==0)
			{
			if (GetTickCount()-dwTickStart > dwDragDelay)
				{
				ReleaseCapture();
				return TRUE;
				}
			}

		MSG msg;
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
			// Move outside rectangle?
			if (msg.message==WM_MOUSEMOVE)
				{
				// Convert to screen coords
				POINT ptMove;
				ptMove.x=LOWORD(msg.lParam);
				ptMove.y=HIWORD(msg.lParam);

				ClientToScreen(msg.hwnd, &ptMove);

				if (!PtInRect(&rc, ptMove))
					{
					ReleaseCapture();
					return TRUE;
					}
				}

			// Mouse up?
			if (msg.message==uMouseUpMessage)
				{
				ReleaseCapture();
				return FALSE;
				}

			// Escape key or mouse up?
			if ((msg.message==WM_KEYDOWN) && (msg.wParam==VK_ESCAPE))
				{
				ReleaseCapture();
				return FALSE;
				}

			// Other input message?
			if ((msg.message>=WM_MOUSEFIRST) && (msg.message<=WM_MOUSELAST))
				continue;
			if ((msg.message>=WM_KEYFIRST) && (msg.message<=WM_KEYLAST))
				continue;
			if ((msg.message>=WM_NCMOUSEMOVE) && (msg.message<=WM_NCMBUTTONDBLCLK))
				continue;

			// Dispatch other messages
			DispatchMessage(&msg);
			}
		else
			MsgWaitForMultipleObjects(0, NULL, TRUE, dwDragDelay/10, QS_ALLEVENTS);
		}
}






}	// namespace Simple
