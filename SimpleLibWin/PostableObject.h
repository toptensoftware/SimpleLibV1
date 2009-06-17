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
// PostableObject.h - declaration of CPostableObject class

#ifndef __POSTABLEOBJECT_H
#define __POSTABLEOBJECT_H

namespace Simple
{

#define WM_POSTABLE_DISPATCH	(WM_USER + 1)
#define WM_POSTABLE_SEND		(WM_USER + 2)
#define WM_POSTABLE_CLOSE		(WM_USER + 3)

// CPostableObject Class			    n
class CPostableObject
{
public:
// Construction
			CPostableObject(bool bAttach=true);
	virtual ~CPostableObject();

// Attributes
	void AttachThread();
	void DetachThread();
	DWORD GetThreadId() { return m_dwThreadId; };

// Operations
	void Post(UINT nMessage, WPARAM wParam=0, LPARAM lParam=0);
	LRESULT PostAndWait(UINT nMessage, WPARAM wParam=0, LPARAM lParam=0);
	LRESULT Send(UINT nMessage, WPARAM wParam=0, LPARAM lParam=0);
	HWND GetDispatchWindow();
	DWORD GetOwningThread() { return m_dwThreadId; };
	bool IsAttachedThread() { return m_dwThreadId==GetCurrentThreadId(); };

// Overrides
	virtual LRESULT OnMessage(UINT nMessage, WPARAM wParam, LPARAM lParam)=0;

private:
	DWORD	m_dwThreadId;
	int		m_iQueuedMessages;

	friend struct POSTABLETHREADINFO;
};

}	// namespace Simple

#endif	// __POSTABLEOBJECT_H

