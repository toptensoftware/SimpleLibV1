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
// MessageLoop.h - declaration of CMessageLoop class

#ifndef __MESSAGELOOP_H
#define __MESSAGELOOP_H


namespace Simple
{

class CMessageLoopFilter
{
public:
	virtual bool PreTranslateMessage(MSG* pMsg)=0;
	virtual void EnableModeless(bool bEnable)=0;
};

class CMessageLoopIdleHandler
{
public:
	virtual bool OnIdle()=0;
};

class CMessageLoopWaitHandler
{
public:
	virtual void WaitMessage()=0;
};

#define MODELESS_HANDLER() \
	if(uMsg == WM_ENABLE) \
	{ \
		m_pMessageLoop->EnableModeless(this, !!wParam); \
	}

// CMessageLoop Class
class CMessageLoop
{
public:
// Construction
			CMessageLoop();
	virtual ~CMessageLoop();

// Attributes
	UINT RunLoop();
	void EndLoop(UINT nRetv);
	void OnIdle();
	void RegisterFilter(CMessageLoopFilter* pFilter);
	void RevokeFilter(CMessageLoopFilter* pFilter);
	void RegisterIdleHandler(CMessageLoopIdleHandler* pHandler);
	void RevokeIdleHandler(CMessageLoopIdleHandler* pHandler);
	void SetWaitHandler(CMessageLoopWaitHandler* pHandler);

	void EnableModeless(CMessageLoopFilter* pOriginator, bool bEnable);

// Operations

// Implementation
protected:
// Attributes
	bool	m_bInLoop;
	bool	m_bEndLoop;
	UINT	m_nRetVal;
	CVector<CMessageLoopFilter*>		m_Filters;
	CVector<CMessageLoopIdleHandler*>	m_IdleHandlers;
	CMessageLoopWaitHandler*			m_pWaitHandler;
	bool	m_bModelessEnabled;
	bool	m_nInModelessOperation;

// Operations

};

}	// namespace Simple


#endif	// __MESSAGELOOP_H

