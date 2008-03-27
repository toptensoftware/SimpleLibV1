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
// TimerVector.h - declaration of CTimerVector

#ifndef __TIMERVECTOR_H
#define __TIMERVECTOR_H

#include "CallbackTimer.h"

namespace Simple
{

class CTimerVectorObject
{
public:
	virtual void OnTimer()=0;
};

class CTimerVector
{
public:
// Construction
			CTimerVector(DWORD dwPeriod=1000);
	virtual ~CTimerVector();

// Operations
	void SetPeriod(DWORD dwMS);
	void Add(CTimerVectorObject* pObject);
	void Remove(CTimerVectorObject* pObject);

	static void CALLBACK TimerProc(LPARAM lUser);

	CVector<CTimerVectorObject*>	m_Objects;
	HCALLBACKTIMER	m_hTimer;
	DWORD			m_dwPeriod;
};



}	// namespace Simple

#endif		// __TIMERVECTOR_H

