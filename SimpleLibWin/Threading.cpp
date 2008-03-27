//////////////////////////////////////////////////////////////////////////
// Threading.cpp - implementation of Threading

#include "stdafx.h"
#include "SimpleLibWinBuild.h"

#include "Threading.h"



namespace Simple
{

/////////////////////////////////////////////////////////////////////////////
// CEvent

// Constructor
CEvent::CEvent() : CSmartHandle<HANDLE>()
{
}

// Constructor
CEvent::CEvent(HANDLE h) : CSmartHandle<HANDLE>(h)
{
}

// Constructor
CEvent::CEvent(bool bManualReset, bool bInitialState)
{
	Create(bManualReset, bInitialState);
}

// Create
BOOL CEvent::Create(bool bManualReset, bool bInitialState)
{
	Release();
	m_hObject=CreateEvent(NULL, bManualReset, bInitialState, NULL);
	ASSERT(m_hObject!=NULL);
	return m_hObject!=NULL;
}


// Set
BOOL CEvent::Set()
{
	ASSERT(m_hObject!=NULL);
	return SetEvent(m_hObject);
}

// Reset
BOOL CEvent::Reset()
{
	ASSERT(m_hObject!=NULL);
	return ResetEvent(m_hObject);
}

// Pulse
BOOL CEvent::Pulse()
{
	ASSERT(m_hObject!=NULL);
	return PulseEvent(m_hObject);
}

// Wait
DWORD CEvent::Wait(DWORD dwMilliseconds)
{
	ASSERT(m_hObject!=NULL);
	return WaitForSingleObject(m_hObject, dwMilliseconds);
}

	
/////////////////////////////////////////////////////////////////////////////
// CSemaphore

CSemaphore::CSemaphore() : CSmartHandle<HANDLE>()
{
}
CSemaphore::CSemaphore(HANDLE h) : CSmartHandle<HANDLE>(h)
{
}

bool CSemaphore::Create(int iInitialCount, int iMaximumCount)
{
	Release();
	m_hObject=CreateSemaphore(NULL, iInitialCount, iMaximumCount, NULL);
	ASSERT(m_hObject!=NULL);
	return m_hObject!=NULL;
}

void CSemaphore::Raise(int iReleaseCount)
{
	::ReleaseSemaphore(m_hObject, iReleaseCount, NULL);
}

DWORD CSemaphore::Wait(DWORD dwMilliseconds)
{
	ASSERT(m_hObject!=NULL);
	return WaitForSingleObject(m_hObject, dwMilliseconds);
}



/////////////////////////////////////////////////////////////////////////////
// CThread

// Constructor
CThread::CThread()
{
	m_hHandle=NULL;
}

// Destructor
CThread::~CThread()
{
	Close();
}

bool CThread::Start()
{
	ASSERT(m_hHandle==NULL);

	// Create the thread...
	m_hHandle=CreateThread(NULL, 0, ThreadProcStub, this, 0, &m_dwID);
	if (!m_hHandle)
		return false;

	return true;
}

void CThread::Close()
{
	if (m_hHandle)
		{
		ASSERT(IsFinished());
		CloseHandle(m_hHandle);
		m_hHandle=NULL;
		}
}

// Stub to launch thread proc
DWORD WINAPI CThread::ThreadProcStub(LPVOID pParam)
{
	// Run the thread...
	CThread* pThread=(CThread*)pParam;
	return pThread->ThreadProc();
}


}	// namespace Simple
