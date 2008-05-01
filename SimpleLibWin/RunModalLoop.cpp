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
// RunModalLoop.cpp - implementation of RunModalLoop

#include "stdafx.h"
#include "SimpleLibWinBuild.h"

#include "RunModalLoop.h"


namespace Simple
{


struct MODALINFO
{
	HWND	m_hWndOwner;
	HWND	m_hWndDisabled;
	int		m_iRetv;
	bool	m_bWasShown;
	bool	m_bFinished;
	MODALINFO*	m_pPrev;
};

int SIMPLEAPI RunModalLoop(HWND hWndOwnerIn, HWND hWndDialog)
{
	// Check for a dialog
	if (!hWndDialog)
		return 0;

	// Cancel capture if any...
	HWND hWndCapture=GetCapture();
	if (hWndCapture)
		SendMessage(hWndCapture, WM_CANCELMODE, 0, 0);

	// Setup modal info
	MODALINFO ModalInfo;
	memset(&ModalInfo, 0, sizeof(MODALINFO));

	// Save previous modal info
	ModalInfo.m_pPrev=(MODALINFO*)GetProp(hWndDialog, L"rml_info");

	// Nested?
	if (!ModalInfo.m_pPrev)
		{
		// Show it
		if (!IsWindowVisible(hWndDialog))
			{
			ShowWindow(hWndDialog, SW_SHOWNORMAL);
			ModalInfo.m_bWasShown=true;
			}

		// Find top most parent of owner
		ModalInfo.m_hWndOwner=hWndOwnerIn;
		while (ModalInfo.m_hWndOwner && GetWindowLong(ModalInfo.m_hWndOwner, GWL_STYLE) & WS_CHILD)
			{
			ModalInfo.m_hWndOwner=GetParent(ModalInfo.m_hWndOwner);
			}

		// Disable the owner?
		ModalInfo.m_hWndDisabled=ModalInfo.m_hWndOwner;

		if (ModalInfo.m_hWndDisabled && (GetWindowLong(ModalInfo.m_hWndDisabled, GWL_STYLE) & WS_DISABLED))
			ModalInfo.m_hWndDisabled=NULL;

		if (ModalInfo.m_hWndDisabled)
			EnableWindow(ModalInfo.m_hWndDisabled, FALSE);
		}
	else
		{
		// Use same owner as previous so idle messages get sent...
		ModalInfo.m_hWndOwner=ModalInfo.m_pPrev->m_hWndOwner;
		}

	// Setup result
	SetProp(hWndDialog, L"rml_info", (HANDLE)&ModalInfo);

	// Do we need to send enter idle message?
	bool bSendEnterIdle=(GetWindowLong(hWndDialog, GWL_STYLE) & DS_NOIDLEMSG)!=0;

	while (true)
		{
		MSG msg;

		// Dialog ended yet?
		if (ModalInfo.m_bFinished)
			break;

		// Get the next message
		while (!PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
			// Send enter idle notification
			if (bSendEnterIdle)
				SendMessage(ModalInfo.m_hWndOwner, WM_ENTERIDLE, MSGF_DIALOGBOX, (LPARAM)hWndDialog);

			// Send idle message
			SendMessage(hWndDialog, WM_IDLE, 0, 0);

			// Wait for a message
			WaitMessage();
			}

		// Handle WM_QUIT
		if (msg.message==WM_QUIT)
			{
			PostQuitMessage(int(msg.wParam));
			if (ModalInfo.m_hWndDisabled)
				EnableWindow(ModalInfo. m_hWndDisabled, TRUE);
			SetProp(hWndDialog, L"rml_info", (HANDLE)&ModalInfo.m_pPrev);
			return 0;
			}

		// Give the dialog itself first crack...
		// Return 1 if processed
		// Return 0 if not processed and do IsDialogMessage (usual case)
		// Other value dispatched messaged but doesn't do IsDialogMessage
		LRESULT lResult=SendMessage(hWndDialog, WM_PRETRANSLATEMESSAGE, 0, (LPARAM)&msg);
		if (lResult==1)
			continue;

		if (lResult==0)
			{
			if (IsDialogMessage(hWndDialog, &msg))
				continue;
			}

		// Final straw...
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		}

	// Restore previous modal info
	SetProp(hWndDialog, L"rml_info", (HANDLE)ModalInfo.m_pPrev);

	// Return result
	return ModalInfo.m_iRetv;
}

void SIMPLEAPI EndModalLoop(HWND hWndDialog, int iResult)
{
	ASSERT(iResult!=0);

	// Get info
	MODALINFO* pModalInfo=(MODALINFO*)GetProp(hWndDialog, L"rml_info");
	if (!pModalInfo->m_pPrev)
		{
		// Reenable parent
		if (pModalInfo->m_hWndDisabled)
			{
			EnableWindow(pModalInfo->m_hWndDisabled, TRUE);
			}

		// Re-activate the owner
		if (pModalInfo->m_hWndOwner)
			SetActiveWindow(pModalInfo->m_hWndOwner);

		// Hide the dialog
		ShowWindow(hWndDialog, SW_HIDE);
		}

	// Setup the result...
	pModalInfo->m_iRetv=iResult;
	pModalInfo->m_bFinished=true;
}




}	// namespace Simple
