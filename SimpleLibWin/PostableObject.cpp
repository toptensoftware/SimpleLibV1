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
// PostableObject.cpp - implementation of CPostableObject class

#include "stdafx.h"
#include "SimpleLibWinBuild.h"

#include "PostableObject.h"
#include "Threading.h"

#pragma warning(disable: 4073)
#pragma init_seg(lib)

namespace Simple
{

//////////////////////////////////////////////////////////////////////////
// CPostableObject

LRESULT CALLBACK WndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);

static const wchar_t* GetClassName()
{
	static wchar_t szBuf[MAX_PATH];
	swprintf(szBuf, _countof(szBuf), L"PostableObjectDispatcher_%.8x", WndProc);
	return szBuf;
}

#define WM_SYSKEYFIRST  WM_SYSKEYDOWN
#define WM_SYSKEYLAST   WM_SYSDEADCHAR
#define WM_NCMOUSEFIRST WM_NCMOUSEMOVE
#define WM_NCMOUSELAST  WM_NCMBUTTONDBLCLK

static bool IsInputMessage(UINT msg)
{
	if ((msg>=WM_MOUSEFIRST) && (msg<=WM_MOUSELAST)) return TRUE;
	if ((msg>=WM_NCMOUSEFIRST) && (msg<=WM_NCMOUSELAST)) return TRUE;
	if ((msg>=WM_KEYFIRST) && (msg<=WM_KEYLAST)) return TRUE;
	if ((msg>=WM_SYSKEYFIRST) && (msg<=WM_SYSKEYLAST)) return TRUE;

	return FALSE;
}


// Information about a single message in the message queue
struct POSTABLEMESSAGE
{
	CPostableObject* pTarget;
	UINT nMessage;
	WPARAM wParam;
	LPARAM lParam;
	HANDLE hReplyEvent;
	LRESULT* pResult;
};


struct POSTABLETHREADINFO
{
public:
	POSTABLETHREADINFO()
	{
		// Init vars
		m_bPostPending=false;
		m_dwCount=0;
		m_bDispatching=false;
		m_hWndDispatcher=NULL;

		// Register window class
		WNDCLASS wc;
		memset(&wc, 0, sizeof(wc));
		wc.lpfnWndProc=WndProc;
		wc.lpszClassName=GetClassName();
		wc.style=CS_GLOBALCLASS;
		RegisterClass(&wc);

		// Create the dispatcher window
		m_hWndDispatcher=CreateWindow(GetClassName(), NULL, WS_CHILD, 0, 0, 0, 0, GetDesktopWindow(), NULL, NULL, NULL);
		ASSERT(m_hWndDispatcher!=NULL);
	}

	~POSTABLETHREADINFO()
	{
		// Destroy the dispatcher window...
		DestroyWindow(m_hWndDispatcher);
	}

	void Dispatch()
	{
		CEnterCriticalSection ecs(&m_Section);

		// This should never go reentrant
		ASSERT(!m_bDispatching);

		// Clear flag
		m_bPostPending=false;

		// Dispatch all pending messages...
		m_bDispatching=true;
		int iCount=m_MessageQueue.GetSize();
		for (int i=0; i<iCount; i++)	
			{
			POSTABLEMESSAGE* p=m_MessageQueue[i];
			if (p->pTarget!=NULL)
				{
				// Dispatch the message, leaving and re-entering the critical section
				ecs.Leave();
				ASSERT(p->pTarget->GetThreadId()==GetCurrentThreadId());
				LRESULT lResult=p->pTarget->OnMessage(p->nMessage, p->wParam, p->lParam);
				if (p->hReplyEvent)
					{
					*p->pResult=lResult;
					SetEvent(p->hReplyEvent);
					}
				ecs.Enter(&m_Section);
				}
			}
		m_bDispatching=false;

		// Remove messages just dispatched...
		m_MessageQueue.RemoveAt(0, iCount);

		// If messages got posted while dispatching messages, post the Windows message now...
		if (m_MessageQueue.GetSize()>0)
			{
			ASSERT(m_bPostPending);
			PostMessage(m_hWndDispatcher, WM_POSTABLE_DISPATCH, 0, 0);
			}
	}

	bool				m_bPostPending;
	DWORD				m_dwCount;
	HWND				m_hWndDispatcher;
	bool				m_bDispatching;
	CCriticalSection	m_Section;
	CVector<POSTABLEMESSAGE*, SOwnedPtr>		m_MessageQueue;
};

// Info about all threads currently holding a postable object
static CMap<DWORD, POSTABLETHREADINFO*, SValue, SOwnedPtr> g_ThreadInfos;
static CCriticalSection g_Section;

// Helper to retrieve info about a thread...
static POSTABLETHREADINFO* GetThreadInfo(DWORD dwThread, bool bCreate)
{
	CEnterCriticalSection ecs(&g_Section);

	POSTABLETHREADINFO* pti=g_ThreadInfos.Get(dwThread, NULL);
	if (pti || !bCreate)
		return pti;

	pti=new POSTABLETHREADINFO();
	g_ThreadInfos.Add(dwThread, pti);
	return pti;
}

// Window procedure for the per thread dispatch window.
static LRESULT CALLBACK WndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	if (nMsg==WM_POSTABLE_DISPATCH)
		{
		POSTABLETHREADINFO* pti=GetThreadInfo(GetCurrentThreadId(), false);
		ASSERT(pti!=NULL);
		pti->Dispatch();
		}
	else if (nMsg==WM_POSTABLE_SEND)
		{
		POSTABLEMESSAGE* p=(POSTABLEMESSAGE*)lParam;
		ASSERT(p->pTarget->GetThreadId()==GetCurrentThreadId());
		return p->pTarget->OnMessage(p->nMessage, p->wParam, p->lParam);
		}
	else if (nMsg==WM_POSTABLE_CLOSE)
		{
		POSTABLETHREADINFO* pti=GetThreadInfo(GetCurrentThreadId(), false);
		if (pti->m_dwCount==0)
			{
			CEnterCriticalSection ecs2(&g_Section);
			g_ThreadInfos.Remove(GetCurrentThreadId());
			}
		}

	return DefWindowProc(hWnd, nMsg, wParam, lParam);
}


// Constructor
CPostableObject::CPostableObject(bool bAttach)
{
	m_dwThreadId=0;
	if (bAttach)
		AttachThread();
}

// Destructor
CPostableObject::~CPostableObject()
{
	DetachThread();
}

void CPostableObject::AttachThread()
{
	// Save the current thread ID
	m_dwThreadId=GetCurrentThreadId();

	// Get info for this thread, creating it if need be
	POSTABLETHREADINFO* pti=GetThreadInfo(m_dwThreadId, true);
	ASSERT(pti!=NULL);

	// Bump the count...
	CEnterCriticalSection ecs(&pti->m_Section);
	pti->m_dwCount++;
}

void CPostableObject::DetachThread()
{
	if (!m_dwThreadId)
		return;

	// Get current thread info
	POSTABLETHREADINFO* pti=GetThreadInfo(m_dwThreadId, false);
	ASSERT(pti!=NULL);

	{
	// Remove any messages for this object from the queue
	CEnterCriticalSection ecs(&pti->m_Section);
	for (int i=pti->m_MessageQueue.GetSize()-1; i>=0; i--)
		{
		// Message for this object?
		if (pti->m_MessageQueue[i]->pTarget==this)
			{
			// Mark as an obsolete message
			pti->m_MessageQueue[i]->pTarget=NULL;
			}
		}

	pti->m_dwCount--;
	if (pti->m_dwCount==0)
		{
		PostMessage(pti->m_hWndDispatcher, WM_POSTABLE_CLOSE, 0, 0);
		}
	}

	m_dwThreadId=0;
}

// Post a message to this object
void CPostableObject::Post(UINT nMessage, WPARAM wParam, LPARAM lParam)
{
	ASSERT(m_dwThreadId!=0);

	// Get thread info
	POSTABLETHREADINFO* pti=GetThreadInfo(m_dwThreadId, false);
	ASSERT(pti!=NULL);

	CEnterCriticalSection ecs(&pti->m_Section);

	// Setup a new message
	POSTABLEMESSAGE* p=new POSTABLEMESSAGE;
	p->pTarget=this;
	p->nMessage=nMessage;
	p->wParam=wParam;
	p->lParam=lParam;
	p->hReplyEvent=NULL;
	p->pResult=NULL;
	pti->m_MessageQueue.Add(p);

	// If no pending Windows message, post one...
	if (!pti->m_bPostPending)
		{
		// Unless we're currently dispatching messages... in which case we'll post at the end of that.
		if (!pti->m_bDispatching)
			{
			PostMessage(pti->m_hWndDispatcher, WM_POSTABLE_DISPATCH, 0, 0);
			}

		// Set flag saying post is pending
		pti->m_bPostPending=true;
		}
}

LRESULT CPostableObject::PostAndWait(UINT nMessage, WPARAM wParam, LPARAM lParam)
{
	ASSERT(m_dwThreadId!=0);

	// Get thread info
	POSTABLETHREADINFO* pti=GetThreadInfo(m_dwThreadId, false);
	ASSERT(pti!=NULL);

	// Setup for response...
	CEvent hReplyEvent(true);
	LRESULT lResult=0;

	{
	CEnterCriticalSection ecs(&pti->m_Section);

	// Setup a new message
	POSTABLEMESSAGE* p=new POSTABLEMESSAGE;
	p->pTarget=this;
	p->nMessage=nMessage;
	p->wParam=wParam;
	p->lParam=lParam;
	p->hReplyEvent=hReplyEvent;
	p->pResult=&lResult;
	pti->m_MessageQueue.Add(p);

	// If no pending Windows message, post one...
	if (!pti->m_bPostPending)
		{
		// Unless we're currently dispatching messages... in which case we'll post at the end of that.
		if (!pti->m_bDispatching)
			{
			PostMessage(pti->m_hWndDispatcher, WM_POSTABLE_DISPATCH, 0, 0);
			}

		// Set flag saying post is pending
		pti->m_bPostPending=true;
		}
	}

	while (true)
		{
		// Dispatch messages...
		MSG msg;
		while (PeekMessage(&msg, 0, 0, NULL, PM_REMOVE))
			{
			if (!IsInputMessage(msg.message))
				{
				DispatchMessage(&msg);
				}
			}

		// Wait for response...
		if (MsgWaitForMultipleObjects(1, &hReplyEvent.m_hObject, FALSE, INFINITE, QS_ALLEVENTS)==WAIT_OBJECT_0)
			return lResult;
		}
}


LRESULT CPostableObject::Send(UINT nMessage, WPARAM wParam, LPARAM lParam)
{
	ASSERT(m_dwThreadId!=0);
	if (GetCurrentThreadId()==m_dwThreadId)
		{
		// Same thread, just call it
		return OnMessage(nMessage, wParam, lParam);
		}
	else
		{
		// Different thread, need to switch to other thread to process the message

		// Get thread info
		POSTABLETHREADINFO* pti=GetThreadInfo(m_dwThreadId, false);
		ASSERT(pti!=NULL);
		
		// Pack the message in a structure
		POSTABLEMESSAGE p;
		p.pTarget=this;
		p.nMessage=nMessage;
		p.wParam=wParam;
		p.lParam=lParam;
		p.hReplyEvent=NULL;

		// Send to the dispatcher window for handling
		return SendMessage(pti->m_hWndDispatcher, WM_POSTABLE_SEND, 0, (LPARAM)&p);
		}
}

HWND CPostableObject::GetDispatchWindow()
{
	ASSERT(m_dwThreadId!=0);
	POSTABLETHREADINFO* pti=GetThreadInfo(m_dwThreadId, false);
	ASSERT(pti!=NULL);
	return pti->m_hWndDispatcher;
}


}	// namespace Simple
