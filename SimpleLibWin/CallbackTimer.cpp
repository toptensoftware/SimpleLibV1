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
// CallbackTimer.cpp - implementation of TimerCallback

#include "stdafx.h"
#include "SimpleLibWinBuild.h"

#include "CallbackTimer.h"

namespace Simple
{

#pragma pack(1)
struct TIMERTHUNK
{
#if defined(_M_IX86)
	DWORD				m_mov;
	TIMERTHUNK*			m_pThunk;
	BYTE				m_jmp;
	int					m_relproc;
#elif defined(_M_X64)
	WORD				m_movrcx;
	LPARAM				m_ecxval;
	WORD				m_movrax;
	LPARAM				m_eaxval;
	WORD				m_jmpeax;
#else
	#error unsupported CPU architecture
#endif


	LPARAM				lUserData;
	CALLBACKTIMERPROC	pfnTimerProc;
	UINT_PTR			nTimerID;
	DWORD				dwFlags;
};
#pragma pack()



static void CALLBACK CallbackTimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	__try
		{
		// Get this pointer
		TIMERTHUNK* pThis=(TIMERTHUNK*)hWnd;

		// One shot?
		if (pThis->dwFlags & CBT_ONESHOT)
			{
			KillTimer(NULL, pThis->nTimerID);
			pThis->nTimerID=0;
			}

		bool bRelease=(pThis->dwFlags & (CBT_ONESHOT|CBT_AUTORELEASE))==(CBT_ONESHOT|CBT_AUTORELEASE);

		// Call the proc
		pThis->pfnTimerProc(pThis->lUserData);

		// Auto release one shot?
		if (bRelease)
			{
			KillCallbackTimer((HCALLBACKTIMER)pThis);
			}
		}
	__except( UnhandledExceptionFilter( GetExceptionInformation() ) )  
		{ 
		}
}

HCALLBACKTIMER SIMPLEAPI SetCallbackTimer(UINT nPeriod, DWORD dwFlags, CALLBACKTIMERPROC pfnCallback, LPARAM lUserData)
{
	// Allocate a thunk
	TIMERTHUNK* pThunk=new TIMERTHUNK;

	DWORD dwTemp;
	VirtualProtect(pThunk, sizeof(*pThunk), PAGE_EXECUTE_READWRITE, &dwTemp);
								
#if defined(_M_IX86)
	pThunk->m_mov = 0x042444C7;  // mov [esp+4]
	pThunk->m_pThunk = pThunk;
	pThunk->m_jmp = 0xe9;
	pThunk->m_relproc = (DWORD)(INT_PTR)CallbackTimerProc - (INT_PTR(pThunk)+offsetof(TIMERTHUNK, lUserData));
#elif defined(_M_X64)
	pThunk->m_movrcx=0xb948;
	pThunk->m_ecxval=(LPARAM)pThunk;
	pThunk->m_movrax=0xb848;
	pThunk->m_eaxval=(LPARAM)CallbackTimerProc;
	pThunk->m_jmpeax=0xE0FF;
#else
	#error unsupported CPU architecture
#endif

	pThunk->lUserData=lUserData;
	pThunk->pfnTimerProc=pfnCallback;
	pThunk->dwFlags=dwFlags;
	pThunk->nTimerID=SetTimer(NULL, 0, nPeriod, (TIMERPROC)pThunk);

	return (HCALLBACKTIMER)pThunk;
}

void SIMPLEAPI KillCallbackTimer(HCALLBACKTIMER hTimer)
{
	TIMERTHUNK* pThunk=(TIMERTHUNK*)hTimer;
	if (!pThunk)
		return;

	if (pThunk->nTimerID)
		KillTimer(NULL, pThunk->nTimerID);

	delete pThunk;
}


}	// namespace Simple

