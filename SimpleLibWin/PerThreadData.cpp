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

#include "PerThreadData.h"

namespace Simple
{

DWORD g_dwPerThreadIndexNext=0;
DWORD g_dwPerThreadCount=0;
DWORD g_dwPerThreadTlsSlot=0;
CPerThreadClassInfo* g_pFirstClassInfo=NULL;


void ReleaseAllThreadData()
{
	CPerThreadClassInfo* p=g_pFirstClassInfo;
	while (p)
	{
		p->m_pfnRelease();
		p=p->m_pNext;
	}
}

}