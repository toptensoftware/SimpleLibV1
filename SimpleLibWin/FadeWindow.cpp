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
// FadeWindow.cpp - implementation of FadeWindow

#include "stdafx.h"
#include "SimpleLibWinBuild.h"

#include "FadeWindow.h"

#include "CallbackTimer.h"
#include "SetLayeredWindowAttributesEx.h"
#include "IsInputMessage.h"

namespace Simple
{
class CFadeInfo
{
public:
// Construction
			CFadeInfo(HWND hWnd, DWORD dwFlags, int iSteps, int iPeriod);
	virtual ~CFadeInfo();

// Operations
	void Finish();

// Attributes
	HWND	m_hWnd;
	DWORD	m_dwFlags;
	int		m_iStep;
	int		m_iSteps;
	int		m_iPeriod;
	HCALLBACKTIMER	m_hTimer;
	bool	m_bFinished;

	void OnTick();

	static void CALLBACK TimerProc(LPARAM lParam)
	{
		((CFadeInfo*)lParam)->OnTick();
	}
};

// List of currently running fade operations
CVector<CFadeInfo*>	g_PendingFades;

// Constructor
CFadeInfo::CFadeInfo(HWND hWnd, DWORD dwFlags, int iSteps, int iPeriod) : 
	m_hWnd(hWnd),
	m_dwFlags(dwFlags),
	m_iStep(0),
	m_iSteps(iSteps),
	m_iPeriod(iPeriod),
	m_bFinished(false)
{
	// Add self to global list...
	g_PendingFades.Add(this);

	// Start a timer
	m_hTimer=SetCallbackTimer(iPeriod, 0, TimerProc, (LPARAM)this);

	// If showing, make sure window is visible
	if (m_dwFlags & FWF_SHOW)
	{
		SetLayeredWindowAttributesEx(m_hWnd, 0xFFFFFFFF, 0, false);
		ShowWindow(m_hWnd, SW_SHOW);
	}
}


// Destructor
CFadeInfo::~CFadeInfo()
{
	KillCallbackTimer(m_hTimer);
	g_PendingFades.Remove(this);
}

// Finish this operation
void CFadeInfo::Finish()
{
	// Hide?
	if (m_dwFlags & FWF_HIDE)
	{
		// Hide the window
		ShowWindow(m_hWnd, SW_HIDE);

		// Remove layering...
		SetLayeredWindowAttributesEx(m_hWnd, 0xFFFFFFFF, 255, false);
	}

	// Destroy?
	if (m_dwFlags & FWF_DESTROY)
	{
		DestroyWindow(m_hWnd);
	}

	// Show?
	if (m_dwFlags & FWF_SHOW)
	{
		// Remove layering...
		SetLayeredWindowAttributesEx(m_hWnd, 0xFFFFFFFF, 255, false);
	}

	// Finished
	if (m_dwFlags & FWF_SYNC)
		m_bFinished=true;
	else
		delete this;
}

void CFadeInfo::OnTick()
{
	// Next step
	m_iStep++;

	//printf("%i of %i\n", m_iStep, m_iSteps);

	// Finished?
	if (m_iStep==m_iSteps)
	{
		Finish();
		return;
	}

	// Work out new opacity
	BYTE bOpacity=m_iStep * 255 / m_iSteps;

	// Swap fade direction for hide
	if ((m_dwFlags & FWF_SHOW)==0)
	{
		bOpacity=255-bOpacity;
	}

	// Set opacity...
	SetLayeredWindowAttributesEx(m_hWnd, 0xFFFFFFFF, bOpacity, true);
}


bool SIMPLEAPI FadeWindow(HWND hWnd, DWORD dwFlags, int iSteps, int iPeriod)
{
	// Kill existing operation
	for (int i=0; i<g_PendingFades.GetSize(); i++)
	{
		if (g_PendingFades[i]->m_hWnd==hWnd)
		{
			g_PendingFades[i]->Finish();
			break;
		}
	}

	// Quit if just cancelling
	if (dwFlags & FWF_CANCEL)
		return true;

	// Quit if not a window, or its a child window
	if (!IsWindow(hWnd) || (GetWindowLong(hWnd, GWL_STYLE) & WS_CHILD))
		return false;

	// Don't use fade effects
	if ((dwFlags & FWF_FORCE)==0 && !UseFadeEffects())
	{
		if (dwFlags & FWF_DESTROY)
		{
			DestroyWindow(hWnd);
			return true;
		}

		if (dwFlags & FWF_SHOW)
		{
			ShowWindow(hWnd, SW_SHOWNORMAL);
			return true;
		}

		if (dwFlags & FWF_HIDE)
		{
			ShowWindow(hWnd, SW_HIDE);
			return true;
		}

		return false;
	}

	// Create new fade entry...
	CFadeInfo* pFade=new CFadeInfo(hWnd, dwFlags, iSteps, iPeriod);
	g_PendingFades.Add(pFade);

	// Wait for finish?
	if (dwFlags & FWF_SYNC)
	{
		while (!pFade->m_bFinished)
		{
			MSG msg;
			if (!GetMessage(&msg, NULL, 0, 0))
				break;

			if (!IsInputMessage(msg.message))
			{
				DispatchMessage(&msg);
			}
		}

		// Delete it
		delete pFade;
	}

	// Done!
	return true;
}


static bool g_bEnableFadeEffects=true;

bool SIMPLEAPI UseFadeEffects()
{
	if (!g_bEnableFadeEffects)
		return false;

	if (GetSystemMetrics(SM_REMOTESESSION))
		return false;

	if (GetSystemMetrics(SM_SLOWMACHINE))
		return false;

	return true;
}

void SIMPLEAPI SetEnableFadeEffects(bool bEnable)
{
	g_bEnableFadeEffects=bEnable;
}

bool SIMPLEAPI GetEnableFadeEffects()
{
	return g_bEnableFadeEffects;
}


}	// namespace Simple
