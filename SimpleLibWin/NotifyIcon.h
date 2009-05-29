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
// NotifyIcon.h - declaration of CNotifyIcon class

#ifndef __NOTIFYICON_H
#define __NOTIFYICON_H

#include <ShellApi.h>

namespace Simple
{

// CNotifyIcon Class
class CNotifyIcon
{
public:
// Construction
			CNotifyIcon();
	virtual ~CNotifyIcon();

// Attributes
	void Create(HWND hWndParent, UINT nID=1, UINT nMessageID=0);
	void Recreate();
	void Delete();
	void Update();
		
	void StartUpdate();
	void EndUpdate();
	bool BalloonsSupported() { return m_bShellV5; };
	void SetIcon(HICON hIcon);
	void SetToolTip(const wchar_t* pszTip);
	void SetBalloonText(const wchar_t* psz);
	void SetBalloonTitle(const wchar_t* psz);
	void SetBalloonFlags(DWORD dwFlags);
	void SetBalloonTimeout(UINT uMilliseconds);

	UINT GetMessageID() { return m_nid.hWnd ? m_nid.uCallbackMessage : 0xFFFFFFFF; };

// Message handler
	LRESULT OnNotifyIcon(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

// Operations
	NOTIFYICONDATA	m_nid;
	DWORD			m_dwBatch;
	bool			m_bShellV5;

// Implementation
protected:
// Attributes

// Operations

};

// Handlers for icon notifications. Eg:
//
//	NOTIFYICON_HANDLER(m_NotifyIcon, WM_LBUTTONDOWN, OnNotifyLButtonDown)

extern const UINT WM_TASKBARCREATED;

#define NOTIFYICON_TASKBARCREATED_HANDLER(nid) \
	if (uMsg==WM_TASKBARCREATED) \
		nid.Recreate();

#define NOTIFYICON_HANDLER(nid, msg, func) \
	if ((nid).m_nid.hWnd && uMsg==(nid).m_nid.uCallbackMessage && wParam==(nid).m_nid.uID && lParam==msg) \
	{ \
		bHandled = TRUE; \
		lResult = func(uMsg, wParam, lParam, bHandled); \
		if(bHandled) \
			return TRUE; \
	}

}	// namespace Simple

#endif	// __NOTIFYICON_H

