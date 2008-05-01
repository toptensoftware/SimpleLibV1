//////////////////////////////////////////////////////////////////////
//
// SimpleLib ATL Win Version 1.0
//
// Copyright (C) 1998-2007 Topten Software.  All Rights Reserved
// http://www.toptensoftware.com
//
// This code has been released for use "as is".  Any redistribution or 
// modification however is strictly prohibited.   See the readme.txt file 
// for complete terms and conditions.
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "SimpleLibAtlWinBuild.h"

#include "RegisteredWindow.h"

namespace Simple
{

#pragma warning(disable:4996)	//'swprintf': swprintf has been changed to conform with the ISO C standard, adding an extra character count parameter. To use traditional Microsoft swprintf, set _CRT_NON_CONFORMING_SWPRINTFS.

ATOM SIMPLEAPI AtlModuleRegisterGlobalWndClassInfo(_ATL_MODULE* pM, _ATL_WNDCLASSINFOW& p, WNDPROC* pProc)
{
	if (p.m_atom == 0)
	{
		_AtlWinModule.m_csWindowCreate.Lock();
		if(p.m_atom == 0)
		{
			HINSTANCE hInst = _AtlBaseModule.GetModuleInstance();
			if (p.m_lpszOrigName != NULL)
			{
				ATLASSERT(pProc != NULL);
				LPCWSTR lpsz = p.m_wc.lpszClassName;
				WNDPROC proc = p.m_wc.lpfnWndProc;

				WNDCLASSEXW wc;
				wc.cbSize = sizeof(WNDCLASSEX);
				// Try global class
				if(!::GetClassInfoExW(NULL, p.m_lpszOrigName, &wc))
				{
					// try process local
					if(!::GetClassInfoExW(_AtlBaseModule.GetModuleInstance(), p.m_lpszOrigName, &wc))
					{
						_AtlWinModule.m_csWindowCreate.Unlock();
						return 0;
					}
				}
				memcpy(&p.m_wc, &wc, sizeof(WNDCLASSEX));
				p.pWndProc = p.m_wc.lpfnWndProc;
				p.m_wc.lpszClassName = lpsz;
				p.m_wc.lpfnWndProc = proc;
			}
			else
			{
				p.m_wc.hCursor = ::LoadCursorW(p.m_bSystemCursor ? NULL : hInst,
					p.m_lpszCursorID);
			}

			p.m_wc.hInstance = hInst;
			p.m_wc.style |= CS_GLOBALCLASS;	// we do register global classes
			if (p.m_wc.lpszClassName == NULL)
			{
#ifdef _WIN64
				swprintf(p.m_szAutoName, L"ATL:%p", &p.m_wc);
#else
				swprintf(p.m_szAutoName, L"ATL:%8.8X", (DWORD_PTR)&p.m_wc);
#endif
				p.m_wc.lpszClassName = p.m_szAutoName;
			}
			WNDCLASSEXW wcTemp;
			memcpy(&wcTemp, &p.m_wc, sizeof(WNDCLASSEXW));
			p.m_atom = (ATOM)::GetClassInfoExW(p.m_wc.hInstance, p.m_wc.lpszClassName, &wcTemp);
			if (p.m_atom == 0)
				p.m_atom = ::RegisterClassExW(&p.m_wc);
		}
		_AtlWinModule.m_csWindowCreate.Unlock();
	}

	if (p.m_lpszOrigName != NULL)
	{
		ATLASSERT(pProc != NULL);
		ATLASSERT(p.pWndProc != NULL);
		*pProc = p.pWndProc;
	}
	return p.m_atom;
}







}	// namespace Simple
