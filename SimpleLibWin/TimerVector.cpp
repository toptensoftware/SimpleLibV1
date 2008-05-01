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
// TimerVector.cpp - implementation of CTimerVector

#include "stdafx.h"
#include "SimpleLibWinBuild.h"

#include "TimerVector.h"

namespace Simple
{


// Constructor
CTimerVector::CTimerVector(DWORD dwPeriod) 
{
	m_hTimer=NULL;
	m_dwPeriod=dwPeriod;
}

// Destructor
CTimerVector::~CTimerVector()
{
	ASSERT(m_Objects.GetSize()==0);
	if (m_hTimer)
		{
		KillCallbackTimer(m_hTimer);
		m_hTimer=NULL;
		}
}

void CTimerVector::SetPeriod(DWORD dwMS)
{
	// Same?
	if (m_dwPeriod==dwMS)
		return;

	// Store new period
	m_dwPeriod=dwMS;

	// Create a new timer...
	if (m_hTimer)
		{
		KillCallbackTimer(m_hTimer);
		m_hTimer=SetCallbackTimer(m_dwPeriod, 0, TimerProc, (LPARAM)this);
		}
}

// Add an object for callbacks
void CTimerVector::Add(CTimerVectorObject* pObject)
{
	// Quit if already added
	if (m_Objects.Find(pObject)>=0)
		return;

	m_Objects.Add(pObject);

	if (m_Objects.GetSize()!=0 && m_hTimer==NULL)
		{
		m_hTimer=SetCallbackTimer(m_dwPeriod, 0, TimerProc, (LPARAM)this);
		}
}

// Remove an object from callbacks
void CTimerVector::Remove(CTimerVectorObject* pObject)
{
	// Find and remove
	int iPos=m_Objects.Find(pObject);
	if (iPos>=0)
		m_Objects.RemoveAt(iPos);

	// Kill Timer?
	if (m_Objects.GetSize()==0 && m_hTimer)
		{
		KillCallbackTimer(m_hTimer);
		m_hTimer=NULL;
		}
}

void CALLBACK CTimerVector::TimerProc(LPARAM lUser)
{
	CTimerVector* pThis=(CTimerVector*)lUser;
	for (int i=pThis->m_Objects.GetSize()-1; i>=0; i--)
		{
		pThis->m_Objects[i]->OnTimer();
		}
}



}	// namespace Simple


