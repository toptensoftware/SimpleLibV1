//////////////////////////////////////////////////////////////////////////
// Threading.h - declaration of Threading

#ifndef __THREADING_H
#define __THREADING_H

#include "SmartHandle.h"

namespace Simple
{

// Critical section wrapper
class CCriticalSection : public CRITICAL_SECTION
{
public:
	CCriticalSection()
	{
		InitializeCriticalSection(this);
	}
	~CCriticalSection()						
	{
		DeleteCriticalSection(this);
	}
#if (_WIN32_WINNT >= 0x0403)
	void SetSpinCount(DWORD dwSpinCount=4000)
	{
		SetCriticalSectionSpinCount(this, dwSpinCount);
	}
#endif
};
class CEnterCriticalSection
{
public:
	CEnterCriticalSection()
	{
		m_pcs=NULL;
	}
	CEnterCriticalSection(CRITICAL_SECTION* pcs)
	{
		m_pcs=NULL;
		Enter(pcs);
	}
	~CEnterCriticalSection()
	{
		Leave();
	}

	void Enter(CRITICAL_SECTION* pcs)
	{
		ASSERT(m_pcs==NULL);
		m_pcs=pcs;
		EnterCriticalSection(pcs);
	}

	void Leave()
	{
		if (m_pcs)
			{
			LeaveCriticalSection(m_pcs);
			m_pcs=NULL;
			}
	}

	CRITICAL_SECTION* m_pcs;
};


// Event wrwapper
class CEvent : public CSmartHandle<HANDLE>
{
public:
// Construction
	CEvent();
	CEvent(HANDLE h);
	CEvent(bool bManualReset, bool bInitialState=false);

// Operations
	BOOL Create(bool bManualReset, bool bInitialState=false);
	BOOL Set();
	BOOL Reset();
	BOOL Pulse();
	DWORD Wait(DWORD dwMilliseconds=INFINITE);
};

// Event wrapper
class CSemaphore : public CSmartHandle<HANDLE>
{
public:
	CSemaphore();
	CSemaphore(HANDLE h);

	bool Create(int iInitialCount, int iMaximumCount);
	void Raise(int iCount);
	DWORD Wait(DWORD dwMilliseconds=INFINITE);
};


class CThread
{
public:
// Construction
			CThread();
	virtual ~CThread();

// Start this thread object
	bool Start();

// Close thread handle
	void Close();

// Operators
	operator HANDLE() 
		{ return m_hHandle; };


// Operations
	DWORD GetID() 
		{ ASSERT(m_hHandle!=NULL); return m_dwID; };

	bool IsFinished()
		{ return Wait(0)==WAIT_OBJECT_0; };


// Win32 wrappers
	DWORD Suspend() 
		{ ASSERT(m_hHandle!=NULL); return SuspendThread(m_hHandle); };

	DWORD Resume() 
		{ ASSERT(m_hHandle!=NULL); return ResumeThread(m_hHandle); };

	DWORD Wait(DWORD dwMilliseconds=INFINITE) 
		{ ASSERT(m_hHandle!=NULL); return WaitForSingleObject(m_hHandle, dwMilliseconds); };

	DWORD GetExitCode()
		{ 
		ASSERT(m_hHandle!=NULL); 
		DWORD dwCode=STILL_ACTIVE; 
		GetExitCodeThread(m_hHandle, &dwCode); 
		return dwCode;
		};


// Implementation
protected:
	HANDLE	m_hHandle;
	DWORD	m_dwID;
	
	virtual DWORD ThreadProc()=0;

	static DWORD WINAPI ThreadProcStub(LPVOID pParam);
};

}	// namespace Simple

#endif	// __THREADING_H

