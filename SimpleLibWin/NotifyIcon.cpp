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
// NotifyIcon.cpp - implementation of CNotifyIcon class

#include "stdafx.h"
#include "SimpleLibWinBuild.h"

#include "NotifyIcon.h"
#include "VersionInfo.h"

namespace Simple
{

#if (_WIN32_IE < 0x500)
#error CNotifyIcon requires _WIN32_IE > 0x0500
#endif

#pragma comment(lib, "shell32.lib")

const UINT WM_TASKBARCREATED = RegisterWindowMessage(L"TaskbarCreated") ;

//////////////////////////////////////////////////////////////////////////
// CNotifyIcon

// Constructor
CNotifyIcon::CNotifyIcon()
{
	m_bShellV5=HIWORD(GetDllVersion(L"shell32.dll"))>=5;

	// Initialise m_nid..
	memset(&m_nid, 0, sizeof(m_nid));
	m_nid.cbSize=m_bShellV5 ? sizeof(m_nid) : NOTIFYICONDATAW_V1_SIZE;
	ASSERT(m_nid.cbSize <= sizeof(m_nid));

	m_dwBatch=0;
}

// Destructor
CNotifyIcon::~CNotifyIcon()
{
	Delete();
}



void CNotifyIcon::Create(HWND hWndParent, UINT nID, UINT nMessageID)
{
	// Delete if already added
	Delete();

	// Check parent window handle is OK
	ASSERT(IsWindow(hWndParent));

	// Message ID not specified, use a default one
	if (nMessageID==0)
		{
		nMessageID=RegisterWindowMessage(L"CNotifyIcon_MessageID");
		}

	// Setup rest of m_nid;
	m_nid.hWnd=hWndParent;
	m_nid.uFlags |= NIF_MESSAGE;
	m_nid.uCallbackMessage=nMessageID;
	m_nid.uID=nID;

	// Add it
	if (!Shell_NotifyIcon(NIM_ADD, &m_nid))
		{
		m_nid.hWnd=NULL;
		}

	m_nid.uFlags=0;
	if (m_bShellV5)
		{
		m_nid.uVersion=NOTIFYICON_VERSION;
//		Shell_NotifyIcon(NIM_SETVERSION, &m_nid);
		m_nid.uTimeout=0;
		}
}

void CNotifyIcon::Recreate()
{
	m_nid.uFlags=NIF_MESSAGE|NIF_ICON|NIF_TIP;
	Create(m_nid.hWnd, m_nid.uID, m_nid.uCallbackMessage);
}

void CNotifyIcon::Delete()
{
	if (m_nid.hWnd)
		{
		Shell_NotifyIcon(NIM_DELETE, &m_nid);
		m_nid.hWnd=NULL;
		}
}

void CNotifyIcon::Update()
{
	if (m_nid.hWnd)
		{
		Shell_NotifyIcon(NIM_MODIFY, &m_nid);
		m_nid.uFlags=0;
		}
}

void CNotifyIcon::StartUpdate()
{
	m_dwBatch++;
}

void CNotifyIcon::EndUpdate()
{
	m_dwBatch--;
	if (!m_dwBatch)
		Update();
}

void CNotifyIcon::SetIcon(HICON hIcon)
{
	StartUpdate();
	m_nid.hIcon=hIcon;
	m_nid.uFlags|=NIF_ICON;
	EndUpdate();
}

void CNotifyIcon::SetToolTip(const wchar_t* pszTip)
{
	StartUpdate();
	lstrcpyn(m_nid.szTip, pszTip, m_bShellV5 ? 128 : 64);
	m_nid.uFlags|=NIF_TIP;
	EndUpdate();
}

void CNotifyIcon::SetBalloonText(const wchar_t* psz)
{
	if (!m_bShellV5)
		{
		if (m_nid.szTip[0]=='\0')
			SetToolTip(psz);
		}
	else
		{
		StartUpdate();
		lstrcpyn(m_nid.szInfo, psz, _countof(m_nid.szInfo));
		m_nid.uFlags|=NIF_INFO;
		EndUpdate();
		}
}

void CNotifyIcon::SetBalloonTitle(const wchar_t* psz)
{
	if (m_bShellV5)
		{
		StartUpdate();
		lstrcpyn(m_nid.szInfoTitle, psz, _countof(m_nid.szInfoTitle));
		m_nid.uFlags|=NIF_INFO;
		EndUpdate();
		}
}

void CNotifyIcon::SetBalloonFlags(DWORD dwFlags)
{
	if (m_bShellV5)
		{
		StartUpdate();
		m_nid.dwInfoFlags=dwFlags;
		m_nid.uFlags|=NIF_INFO;
		EndUpdate();
		}
}

void CNotifyIcon::SetBalloonTimeout(UINT uMilliseconds)
{
	if (m_bShellV5)
		{
		StartUpdate();
		m_nid.uTimeout=uMilliseconds;
		m_nid.uFlags|=NIF_INFO;
		EndUpdate();
		}
}

// Message handler
LRESULT CNotifyIcon::OnNotifyIcon(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (!m_nid.hWnd || uMsg!=m_nid.uCallbackMessage || wParam!=m_nid.uID)
		{
		bHandled=FALSE;
		return 0;
		}

	/*
	*/


	return 0;
}

/*
SetForegroundWindow(hDlg);

   // Display the menu
   TrackPopupMenu(   hSubMenu,
                     TPM_RIGHTBUTTON,
                     pt.x,
                     pt.y,
                     0,
                     hDlg,
                     NULL);

   PostMessage(hDlg, WM_NULL, 0, 0);

   */

}	// namespace Simple
