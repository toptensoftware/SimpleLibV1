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
// SimpleDdeClient.cpp - implementation of SimpleDdeClient

#include "stdafx.h"
#include "SimpleLibWinBuild.h"

#include "SimpleDdeClient.h"

#include <dde.h>

namespace Simple
{

//////////////////////////////////////////////////////////////////////////
// CSimpleDdeClient

// Constructor
CSimpleDdeClient::CSimpleDdeClient()
{
	m_hAtomApp=NULL;
	m_hAtomTopic=NULL;
}

// Destructor
CSimpleDdeClient::~CSimpleDdeClient()
{
	Close();
}

// Initialise this DDE client
HRESULT CSimpleDdeClient::Init(const wchar_t* pszAppName, const wchar_t* pszTopic)
{
	ASSERT(m_hAtomApp==NULL);
	ASSERT(m_hAtomTopic==NULL);

	m_hAtomApp=GlobalAddAtom(pszAppName);
	m_hAtomTopic=GlobalAddAtom(pszTopic);
	return S_OK;	
}

// Close DDE client
HRESULT CSimpleDdeClient::Close()
{
	if (m_hAtomApp)
		GlobalDeleteAtom(m_hAtomApp);
	if (m_hAtomTopic)
		GlobalDeleteAtom(m_hAtomTopic);
	m_hAtomApp=NULL;
	m_hAtomTopic=NULL;
	return S_OK;
}


LRESULT CSimpleDdeClient::DdeMessageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled=FALSE;

	switch (msg)
		{
		case WM_DDE_INITIATE:
			bHandled=TRUE;
			return OnDDEInitiate(hWnd, wParam, lParam);

		case WM_DDE_EXECUTE:
			bHandled=TRUE;
			return OnDDEExecute(hWnd, wParam, lParam);

		case WM_DDE_TERMINATE:
			bHandled=TRUE;
			return OnDDETerminate(hWnd, wParam, lParam);
		}

	return 0;
}

// WM_DDE_INITIATE handler
LRESULT CSimpleDdeClient::OnDDEInitiate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	// Quit if not initialised
	if (!m_hAtomApp)
		return 0;

	// Quit if wrong app/topic specified
	if (LOWORD(lParam)!=m_hAtomApp || HIWORD(lParam)!=m_hAtomTopic)
		return 0;
	
	// make duplicates of the incoming atoms (really adding a reference)
	TCHAR szAtomName[_MAX_PATH];
	GlobalGetAtomName(m_hAtomApp, szAtomName, _MAX_PATH - 1);
	GlobalAddAtom(szAtomName);
	GlobalGetAtomName(m_hAtomTopic, szAtomName, _MAX_PATH - 1);
	GlobalAddAtom(szAtomName);

	// send the WM_DDE_ACK (caller will delete duplicate atoms)
	::SendMessage((HWND)wParam, WM_DDE_ACK, (WPARAM)hWnd,
		MAKELPARAM(m_hAtomApp, m_hAtomTopic));
	return 0L;
}

LRESULT CSimpleDdeClient::OnDDEExecute(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	// unpack the DDE message
	UINT_PTR unused;
	HGLOBAL hData;
	UnpackDDElParam(WM_DDE_EXECUTE, lParam, &unused, (PUINT_PTR)&hData);

	// get the command string
	TCHAR szCommand[_MAX_PATH * 2];
	const wchar_t* lpsz = (const wchar_t*)GlobalLock(hData);
	lstrcpyn(szCommand, lpsz, _countof(szCommand));
	GlobalUnlock(hData);

	// acknowledge now - before attempting to execute
	::PostMessage((HWND)wParam, WM_DDE_ACK, (WPARAM)hWnd,
		ReuseDDElParam(lParam, WM_DDE_EXECUTE, WM_DDE_ACK,
		(UINT)0x8000, (UINT_PTR)hData));

	// don't execute the command when the window is disabled
	if (!IsWindowEnabled(hWnd))
	{
		return 0;
	}

	// execute the command
	OnDDECommand(szCommand);

	return 0L;
}

LRESULT CSimpleDdeClient::OnDDETerminate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	::PostMessage((HWND)wParam, WM_DDE_TERMINATE, (WPARAM)hWnd, lParam);
	return 0;
}


}