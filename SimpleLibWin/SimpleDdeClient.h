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
// SimpleDdeClient.h - declaration of SimpleDdeClient

#ifndef __SIMPLEDDECLIENT_H
#define __SIMPLEDDECLIENT_H


namespace Simple
{

// CSimpleDdeClient Class
class CSimpleDdeClient
{
public:
// Construction
			CSimpleDdeClient();
	virtual ~CSimpleDdeClient();

// Attributes
	HRESULT Init(const wchar_t* pszAppName, const wchar_t* pszTopic);
	HRESULT Close();
	LRESULT DdeMessageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
// Message handlers
	LRESULT OnDDEInitiate(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT OnDDEExecute(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT OnDDETerminate(HWND hWnd, WPARAM wParam, LPARAM lParam);

// Attributes
	ATOM	m_hAtomApp;
	ATOM	m_hAtomTopic;

// Overrides
	virtual void OnDDECommand(const wchar_t* psz)=0;
};

}	// namespace Simple

#endif	// __SIMPLEDDECLIENT_H

