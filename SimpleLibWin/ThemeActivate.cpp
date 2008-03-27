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


#include "stdafx.h"
#include "SimpleLibWinBuild.h"

#include "ThemeActivate.h"

namespace Simple
{


/////////////////////////////////////////////////////////////////////////////
// Redefines of stuff from Platform SDK (save having to include entire SDK)

typedef struct tagMYACTCTX {
  ULONG   cbSize;
  DWORD   dwFlags;
  const wchar_t* lpSource;
  USHORT  wProcessorArchitecture;
  LANGID  wLangId;
  const wchar_t* lpAssemblyDirectory;
  const wchar_t* lpResourceName;
  const wchar_t* lpApplicationName;
  HMODULE hmodule;
} MYACTCTX, *PMYACTCTX;

typedef HANDLE (STDAPICALLTYPE * LPFNCREATEACTCTX)(PMYACTCTX);
typedef BOOL (STDAPICALLTYPE * LPFNACTIVATEACTCTX)(HANDLE, ULONG_PTR*);
typedef BOOL (STDAPICALLTYPE * LPFNDEACTIVATEACTCTX)(HANDLE, ULONG_PTR);
typedef VOID (STDAPICALLTYPE * LPFNRELEASEACTCTX)(HANDLE);

#ifndef ACTCTX_FLAG_RESOURCE_NAME_VALID
#define ACTCTX_FLAG_RESOURCE_NAME_VALID	0x8
#endif

/////////////////////////////////////////////////////////////////////////////
// Global vars

HANDLE					g_hActCtx;
int						g_resourceID;
LPFNCREATEACTCTX		g_pfnCreateActCtx;
LPFNACTIVATEACTCTX		g_pfnActivateActCtx;
LPFNDEACTIVATEACTCTX	g_pfnDeactivateActCtx;
LPFNRELEASEACTCTX		g_pfnReleaseActCtx;
HMODULE					g_hKernel32;



void InitThemeActivateImpl(HMODULE hMod, int id)
{
	g_hActCtx=INVALID_HANDLE_VALUE;
	g_pfnCreateActCtx=NULL;
	g_pfnActivateActCtx=NULL;
	g_pfnDeactivateActCtx=NULL;
	g_pfnReleaseActCtx=NULL;

	g_hKernel32=LoadLibrary(L"kernel32.dll");
	if (g_hKernel32)	
		{
		#ifdef _UNICODE
		g_pfnCreateActCtx=(LPFNCREATEACTCTX)GetProcAddress(g_hKernel32, "CreateActCtxW");
		#else
		g_pfnCreateActCtx=(LPFNCREATEACTCTX)GetProcAddress(g_hKernel32, "CreateActCtxA");
		#endif
		g_pfnActivateActCtx=(LPFNACTIVATEACTCTX)GetProcAddress(g_hKernel32, "ActivateActCtx");
		g_pfnDeactivateActCtx=(LPFNDEACTIVATEACTCTX)GetProcAddress(g_hKernel32, "DeactivateActCtx");
		g_pfnReleaseActCtx=(LPFNRELEASEACTCTX)GetProcAddress(g_hKernel32, "ReleaseActCtx");
		}

	// Pre XP OS?
	if (!g_pfnCreateActCtx)
		return;

	// Create activation context
	TCHAR szPath[MAX_PATH];
	if (0 == GetModuleFileName(hMod, szPath, MAX_PATH))
		return;
	MYACTCTX act = {0};
	act.dwFlags = ACTCTX_FLAG_RESOURCE_NAME_VALID;
	act.lpResourceName = MAKEINTRESOURCE(id);
	act.cbSize = sizeof(act);
	act.lpSource = szPath;
	g_hActCtx = g_pfnCreateActCtx(&act);
}

void CleanUpThemeActivateImpl()
{
	if (g_hActCtx!=INVALID_HANDLE_VALUE)
		{
		g_pfnReleaseActCtx(g_hActCtx);
		g_hActCtx = INVALID_HANDLE_VALUE;
		}

	if (g_hKernel32)
		FreeLibrary(g_hKernel32);
}

void ThemeActivate(HANDLE hContext, ULONG_PTR* pulCookie)
{
	if (!g_pfnActivateActCtx)
		return;

	g_pfnActivateActCtx(hContext, pulCookie); 
}

void ThemeActivate(ULONG_PTR* pulCookie)
{
	ThemeActivate(g_hActCtx, pulCookie);
}

void ThemeDeactivate(ULONG_PTR ulCookie)
{
	if (!g_pfnDeactivateActCtx)
		return;

	g_pfnDeactivateActCtx(0, ulCookie);
}




}	// namespace Simple
