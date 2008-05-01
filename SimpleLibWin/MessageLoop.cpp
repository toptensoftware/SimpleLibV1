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
// MessageLoop.cpp - implementation of CMessageLoop class

#include "stdafx.h"
#include "SimpleLibWinBuild.h"

#include "MessageLoop.h"

namespace Simple
{

//////////////////////////////////////////////////////////////////////////
// CMessageLoop

// Constructor
CMessageLoop::CMessageLoop()
{
	m_bEndLoop=false;
	m_bInLoop=false;
	m_nRetVal=0;
	m_pWaitHandler=NULL;
	m_bModelessEnabled=true;
	m_nInModelessOperation=false;
}

// Destructor
CMessageLoop::~CMessageLoop()
{
}


// Implementation of RunLoop
UINT CMessageLoop::RunLoop()
{
	bool bWasInLoop=m_bInLoop;
	bool bWasEndLoop=m_bEndLoop;

	m_bInLoop=true;
	m_bEndLoop=false;

	while (true)
		{
		MSG msg;

		// EndLoop?
		if (m_bEndLoop)
			break;

		while (!PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
			// Call idle handlers...
			OnIdle();

			if (m_pWaitHandler)
				m_pWaitHandler->WaitMessage();
			else
				WaitMessage();
			}

		if (msg.message==WM_QUIT)
			{
			m_nRetVal=(UINT)msg.wParam;
			break;
			}


		// Call filters...
		bool bHandled=false;
		for (int i=0; i<m_Filters.GetSize(); i++)
			{
			if (m_Filters[i]->PreTranslateMessage(&msg))
				{
				bHandled=true;
				break;
				}
			}

		// If still not handled, translate and dispatch as per normal...
		if (!bHandled)
			{
			// Final straw...
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			}
		}

	UINT nRetv=m_nRetVal;

	m_bInLoop=bWasInLoop;
	m_bEndLoop=bWasEndLoop;

	return nRetv;
}

// Implementation of EndLoop
void CMessageLoop::EndLoop(UINT nRetVal)
{
	ASSERT(m_bInLoop);
	ASSERT(!m_bEndLoop);
	m_bEndLoop=true;
	m_nRetVal=nRetVal;
}

// Implementation of OnIdle
void CMessageLoop::OnIdle()
{
	// Call all idle handlers
	for (int i=0; i<m_IdleHandlers.GetSize(); i++)
		{
		m_IdleHandlers[i]->OnIdle();
		}
}

// Implementation of RegisterFilter
void CMessageLoop::RegisterFilter(CMessageLoopFilter* pFilter)
{
	ASSERT(m_Filters.Find(pFilter)<0);
	m_Filters.Add(pFilter);
}

// Implementation of RevokeFilter
void CMessageLoop::RevokeFilter(CMessageLoopFilter* pFilter)
{
	ASSERT(m_Filters.Find(pFilter)>=0);
	m_Filters.Remove(pFilter);
}

// Implementation of RegisterIdleHandler
void CMessageLoop::RegisterIdleHandler(CMessageLoopIdleHandler* pHandler)
{
	ASSERT(m_IdleHandlers.Find(pHandler)<0);
	m_IdleHandlers.Add(pHandler);
}

// Implementation of RevokeIdleHandler
void CMessageLoop::RevokeIdleHandler(CMessageLoopIdleHandler* pHandler)
{
	ASSERT(m_IdleHandlers.Find(pHandler)>=0);
	m_IdleHandlers.Remove(pHandler);
}

void CMessageLoop::SetWaitHandler(CMessageLoopWaitHandler* pHandler)
{
	m_pWaitHandler=pHandler;
}

void CMessageLoop::EnableModeless(CMessageLoopFilter* pOriginator, bool bEnable)
{
	if (!this)
		return;

	for (int i=0; i<m_Filters.GetSize(); i++)
		{
		if (m_Filters[i]!=pOriginator)
			m_Filters[i]->EnableModeless(bEnable);
		}
}

}	// namespace Simple
