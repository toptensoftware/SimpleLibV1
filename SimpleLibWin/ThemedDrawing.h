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
// ThemedDrawing.h - declaration of ThemedDrawing class

#ifndef __THEMEDDRAWING_H
#define __THEMEDDRAWING_H

#include <uxtheme.h>
#if _WIN32_WINNT>=0x0600
#include <vssym32.h>
#else
#include <tmschema.h>
#endif

namespace Simple
{

// Entry point type defs
typedef HRESULT (STDAPICALLTYPE* PFNGETTHEMEBACKGROUNDCONTENTRECT)(HTHEME, HDC, int, int, LPCRECT, LPRECT);
typedef HRESULT (STDAPICALLTYPE* PFNDRAWTHEMEBACKGROUND)(HTHEME, HDC, int, int, LPCRECT, LPCRECT);
typedef HRESULT (STDAPICALLTYPE* PFNDRAWTHEMEEDGE)(HTHEME, HDC, int, int, LPCRECT, UINT, UINT, LPRECT);
typedef HRESULT (STDAPICALLTYPE* PFNDRAWTHEMETEXT)(HTHEME , HDC, int, int, LPCWSTR, int, DWORD, DWORD, LPCRECT );
typedef HTHEME (STDAPICALLTYPE* PFNGETWINDOWTHEME)(HWND);
typedef HTHEME (STDAPICALLTYPE* PFNSETWINDOWTHEME)(HWND, LPCWSTR, LPCWSTR);
typedef HTHEME (STDAPICALLTYPE* PFNOPENTHEMEDATA)(HWND, LPCWSTR);
typedef BOOL (STDAPICALLTYPE* PFNISTHEMEACTIVE)();
typedef HRESULT (STDAPICALLTYPE* PFNCLOSETHEMEDATA)(HTHEME);

// Entry points
extern PFNGETTHEMEBACKGROUNDCONTENTRECT pfnGetThemeBackgroundContentRect;
extern PFNDRAWTHEMEBACKGROUND pfnDrawThemeBackground;
extern PFNDRAWTHEMEEDGE pfnDrawThemeEdge;
extern PFNDRAWTHEMETEXT pfnDrawThemeText;
extern PFNGETWINDOWTHEME pfnGetWindowTheme;
extern PFNSETWINDOWTHEME pfnSetWindowTheme;
extern PFNOPENTHEMEDATA pfnOpenThemeData;
extern PFNISTHEMEACTIVE pfnIsThemeActive;
extern PFNCLOSETHEMEDATA pfnCloseThemeData;


// Helper to make sure uxtheme.dll loaded
HINSTANCE SIMPLEAPI LoadThemeLib();


// CThemeData - loads an automatically frees a HTHEME
class CThemeData
{
public:
// Construction
	CThemeData();
	CThemeData(HWND hWnd, LPCWSTR pszClassList);
	~CThemeData();

// Operations
	HTHEME Open(HWND hWnd, LPCWSTR pszClassList);
	void Close();

// Operators
	operator HTHEME() { return m_hTheme; };
	bool operator !() { return m_hTheme==NULL; };

// Attributes
	HTHEME m_hTheme;
};



// States supported by DrawThemedXXX
#define THEME_NORMAL		0
#define THEME_PRESSED		DFCS_PUSHED
#define THEME_HOT			DFCS_HOT
#define THEME_DISABLED		DFCS_INACTIVE
#define THEME_HIDEPREFIX	0x80000000
#define THEME_DEFAULTED		0x40000000
#define THEME_FOCUSED		0x20000000

// Draw a themed button using above styles.  pszText=NULL for no caption...
void SIMPLEAPI DrawThemedButton(CThemeData& theme, HDC hDC, LPCRECT prc, LPCWSTR pszText, UINT uState, int iIndent=-1);
void SIMPLEAPI DrawThemedButton(HWND hWnd, HDC hDC, LPCRECT prc, LPCWSTR pszText, UINT uState, int iIndent=-1);

// Draw a themed toolbar button
void SIMPLEAPI DrawThemedToolBarButton(CThemeData& theme, HDC hDC, LPCRECT prc, LPCWSTR pszText, UINT uState, HIMAGELIST hImageList, int iImage);
void SIMPLEAPI DrawThemedToolBarButton(HWND hWnd, HDC hDC, LPCRECT prc, LPCWSTR pszText, UINT uState, HIMAGELIST hImageList, int iImage);

}	// namespace Simple

#endif	// __THEMEDDRAWING_H

