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
// ThemedDrawing.cpp - implementation of ThemedDrawing class

#include "stdafx.h"
#include "SimpleLibWinBuild.h"

#include "ThemedDrawing.h"
#include "GdiText.h"
#include "GdiMisc.h"

namespace Simple
{


/////////////////////////////////////////////////////////////////////////////
// Misc helpers


#ifndef DT_HIDEPREFIX
#define DT_HIDEPREFIX               0x00100000
#endif

static void FillRect(HDC hDC, int l, int t, int r, int b, HBRUSH hbr)
{
	RECT rc;
	SetRect(&rc, l, t, r, b);
	::FillRect(hDC, &rc, hbr);
}



/////////////////////////////////////////////////////////////////////////////
// Helper class to load uxtheme.dll

PFNGETTHEMEBACKGROUNDCONTENTRECT pfnGetThemeBackgroundContentRect=NULL;
PFNDRAWTHEMEBACKGROUND pfnDrawThemeBackground=NULL;
PFNDRAWTHEMEEDGE pfnDrawThemeEdge=NULL;
PFNDRAWTHEMETEXT pfnDrawThemeText=NULL;
PFNGETWINDOWTHEME pfnGetWindowTheme=NULL;
PFNSETWINDOWTHEME pfnSetWindowTheme=NULL;
PFNOPENTHEMEDATA pfnOpenThemeData=NULL;
PFNISTHEMEACTIVE pfnIsThemeActive=NULL;
PFNCLOSETHEMEDATA pfnCloseThemeData=NULL;

class CThemeLib
{
public:
			CThemeLib();
	virtual ~CThemeLib();

	HINSTANCE Load();

	bool m_bLoaded;
	HINSTANCE m_hLib;
};

// Constructor
CThemeLib::CThemeLib()
{
	m_bLoaded=false;
	m_hLib=NULL;
}

// Destructor
CThemeLib::~CThemeLib()
{
	if (m_hLib)
		FreeLibrary(m_hLib);
}

HINSTANCE CThemeLib::Load()
{
	// Already loaded
	if (m_hLib)
		return m_hLib;

	// Previous load attempt failed?
	if (m_bLoaded)
		return NULL;

	// Remember load attempt
	m_bLoaded=true;
		
	// Load the library
	m_hLib=LoadLibrary(L"uxtheme.dll");
	if (!m_hLib)
		return NULL;

	// Get proc addresses
	pfnGetThemeBackgroundContentRect=(PFNGETTHEMEBACKGROUNDCONTENTRECT)GetProcAddress(m_hLib, "GetThemeBackgroundContentRect");
	pfnDrawThemeBackground=(PFNDRAWTHEMEBACKGROUND)GetProcAddress(m_hLib, "DrawThemeBackground");
	pfnDrawThemeEdge=(PFNDRAWTHEMEEDGE)GetProcAddress(m_hLib, "DrawThemeEdge");
	pfnDrawThemeText=(PFNDRAWTHEMETEXT)GetProcAddress(m_hLib, "DrawThemeText");
	pfnGetWindowTheme=(PFNGETWINDOWTHEME)GetProcAddress(m_hLib, "GetWindowTheme");
	pfnSetWindowTheme=(PFNSETWINDOWTHEME)GetProcAddress(m_hLib, "SetWindowTheme");
	pfnOpenThemeData=(PFNOPENTHEMEDATA)GetProcAddress(m_hLib, "OpenThemeData");
	pfnIsThemeActive=(PFNISTHEMEACTIVE)GetProcAddress(m_hLib, "IsThemeActive");
	pfnCloseThemeData=(PFNCLOSETHEMEDATA)GetProcAddress(m_hLib, "CloseThemeData");


	return m_hLib;
}



/////////////////////////////////////////////////////////////////////////////
// LoadThemeLib implementation

static CThemeLib g_ThemeLib;
HINSTANCE LoadThemeLib()
{
	return g_ThemeLib.Load();
}

/////////////////////////////////////////////////////////////////////////////
// CThemeData

// Constructor
CThemeData::CThemeData()
{
	m_hTheme=NULL;
}

// Constructor
CThemeData::CThemeData(HWND hWnd, LPCWSTR pszClassList)
{
	m_hTheme=NULL;
	Open(hWnd, pszClassList);
}

// Destructor
CThemeData::~CThemeData()
{
	Close();
}

// Open implementation
HTHEME CThemeData::Open(HWND hWnd, LPCWSTR pszClassList)
{
	Close();
	if (LoadThemeLib() && pfnIsThemeActive())
		{
		m_hTheme=pfnOpenThemeData(hWnd, pszClassList);
		return m_hTheme;
		}

	return NULL;

}

// Close implementation
void CThemeData::Close()
{
	if (m_hTheme)
		{
		ASSERT(LoadThemeLib()!=NULL);
		pfnCloseThemeData(m_hTheme);		
		}
}



/////////////////////////////////////////////////////////////////////////////
// DrawThemedButton

// Draw themed XP style button
void DrawThemedButton(CThemeData& theme, HDC hDC, LPCRECT prc, LPCWSTR pszText, UINT uState, int iIndent)
{
	// Work out text flags
	UINT dwDrawText=DT_VCENTER|DT_SINGLELINE|DT_WORD_ELLIPSIS;
	if (uState & THEME_HIDEPREFIX)
		dwDrawText |= DT_HIDEPREFIX;

	if (iIndent<0)
		dwDrawText|=DT_CENTER;
	else
		dwDrawText|=DT_LEFT;

	if (theme)
		{
		int iState=PBS_NORMAL;
		if (uState & THEME_DEFAULTED)
			iState=PBS_DEFAULTED;
		if (uState & THEME_HOT)			
			iState=PBS_HOT;
		if (uState & THEME_PRESSED)		
			iState=PBS_PRESSED;
		if (uState & THEME_DISABLED)
			iState=PBS_DISABLED;

		// Draw button background
		pfnDrawThemeBackground(theme, hDC, BP_PUSHBUTTON, iState, prc, NULL);

		RECT rcContent;
		pfnGetThemeBackgroundContentRect(theme, hDC, BP_PUSHBUTTON, iState, prc, &rcContent);

		if (uState & THEME_FOCUSED)
			{
			DrawFocusRect(hDC, &rcContent);
			}

		// Draw text
		if (pszText)
			{
			if (iIndent>=0)
				rcContent.left+=iIndent;

			pfnDrawThemeText(theme, hDC, BP_PUSHBUTTON, iState, pszText, -1, dwDrawText, iState==PBS_DISABLED ? DTT_GRAYED : 0, &rcContent);
			}
		}
	else
		{
		RECT rc=*prc;

		if (uState & THEME_DEFAULTED)
			{
			InflateRect(&rc, -1, -1);
			HBRUSH hbr=GetSysColorBrush(COLOR_WINDOWFRAME);
			FrameRect(hDC, &rc, hbr);
			InflateRect(&rc, -1, -1);
			}

		// Old style
		DrawFrameControl(hDC, &rc, DFC_BUTTON, DFCS_BUTTONPUSH|(uState & 0x0000FFFF));

		// Draw text...
		if (pszText)
			{
			RECT rcContent=rc;
			if (iIndent>=0)
				{
				rcContent.left+=iIndent+2;
				}

			COLORREF rgbOldText=SetTextColor(hDC, GetSysColor( (uState & THEME_DISABLED) ? COLOR_GRAYTEXT : COLOR_BTNTEXT));
			int iOldMode=SetBkMode(hDC, TRANSPARENT);

			if (uState & THEME_PRESSED)
				OffsetRect(&rcContent, 1, 1);

			DrawText(hDC, pszText, -1, &rcContent, dwDrawText);

			SetTextColor(hDC, rgbOldText);
			SetBkMode(hDC, iOldMode);
			}

		// Focus?
		if (uState & THEME_FOCUSED)
			{
			InflateRect(&rc, -2, -2);
			DrawFocusRect(hDC, &rc);
			}
		}
}

void DrawThemedButton(HWND hWnd, HDC hDC, LPCRECT prc, LPCWSTR pszText, UINT uState, int iIndent)
{
	CThemeData theme;
	theme.Open(hWnd, L"Button");
	DrawThemedButton(theme, hDC, prc, pszText, uState, iIndent);
}



#ifndef DT_HIDEPREFIX
#define DT_HIDEPREFIX               0x00100000
#endif
#define THEME_CHECKED DFCS_CHECKED

#define CY_BUTTONMARGIN		3			// Gap between icon/text and top/bottom of button
#define CX_BUTTONMARGIN		4			// Gap between icon/text and left/right of button
#define CX_ICONTEXTGAP		4			// Gap between icon and text
#define CX_TEXTRIGHTMARGIN	3			// Gap to right of text
#define CX_SEPARATOR		5			// Total width of a separator
#define CY_SEPARATOR		5			// Total height of a separator


// Draw themed XP style button
void DrawThemedToolBarButton(CThemeData& theme, HDC hDC, LPCRECT prc, LPCWSTR pszText, 
					UINT uState, HIMAGELIST hImageList, int iImage)
{
	// Work out text flags
	UINT dwDrawText=DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_WORD_ELLIPSIS;
	if (uState & THEME_HIDEPREFIX)
		dwDrawText |= DT_HIDEPREFIX;

	RECT rc=*prc;

	if (theme)
		{
		int iState=TS_NORMAL;
		if (uState & THEME_HOT)			
			iState=TS_HOT;
		if (uState & THEME_CHECKED)
			iState=TS_CHECKED;
		if ((uState & (THEME_CHECKED|THEME_HOT))==(THEME_CHECKED|THEME_HOT))
			iState=TS_HOTCHECKED;
		if (uState & THEME_PRESSED)		
			iState=TS_PRESSED;
		if (uState & THEME_DISABLED)
			iState=TS_DISABLED;

		// Draw button background
		pfnDrawThemeBackground(theme, hDC, TP_BUTTON, iState, &rc, NULL);

		if (uState & (THEME_PRESSED|THEME_CHECKED))
			OffsetRect(&rc, 1, 1);

		if (hImageList!=NULL && iImage>=0)
			{
			int cxIcon, cyIcon;
			ImageList_GetIconSize(hImageList, &cxIcon, &cyIcon);

			if ((uState & THEME_DISABLED)==0)
				{
				ImageList_Draw(hImageList, iImage, hDC, 
						rc.left + CX_BUTTONMARGIN,
						rc.top+ CY_BUTTONMARGIN, ILD_TRANSPARENT);
				}
			else
				{
				HICON hIcon=ImageList_GetIcon(hImageList, iImage, 0);
				DrawState(hDC, NULL, NULL, (LPARAM)hIcon, 0, rc.left + CX_BUTTONMARGIN, rc.top+ CY_BUTTONMARGIN, cxIcon, cyIcon, DST_ICON|DSS_DISABLED);
				DestroyIcon(hIcon);
				}

			rc.left=rc.left + cxIcon + CX_BUTTONMARGIN + CX_ICONTEXTGAP;
			}
		else
			{
			rc.left +=CX_BUTTONMARGIN+CX_TEXTRIGHTMARGIN/2+1;
			}


		if (!IsEmptyString(pszText))
			{
			TEXTMETRIC tm;
			GetTextMetrics(hDC, &tm);

			rc.top=rc.top + (rc.bottom-rc.top-tm.tmHeight)/2;
			rc.right-=5;

			if (uState & THEME_DISABLED)
				{
				DrawState(hDC, NULL, NULL, (LPARAM)pszText, 0, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, DST_PREFIXTEXT|DSS_DISABLED);
				}
			else
				{
				SetTextColor(hDC, COLOR_BTNTEXT);
				SlxDrawText(hDC, pszText, -1, &rc, DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
				}
			}
		}
	else
		{
		if (uState & (THEME_PRESSED|THEME_CHECKED))
			{
			RECT rcFill=rc;
			DrawEdge(hDC, &rcFill, BDR_SUNKENOUTER, BF_RECT|BF_ADJUST);

			if ((uState & (THEME_PRESSED|THEME_CHECKED))==THEME_CHECKED)
				{
				InflateRect(&rcFill, -1, -1);
				SetTextColor(hDC, GetSysColor(COLOR_BTNHIGHLIGHT));
				SetBkColor(hDC, GetSysColor(COLOR_BTNFACE));

				// Create a brush
				HBRUSH hbr=CreateDitherBrush();
				::FillRect(hDC, &rcFill, hbr);
				DeleteObject(hbr);
				}

			OffsetRect(&rc, 1, 1);
			}
		else if ((uState & (THEME_DISABLED|THEME_HOT))==THEME_HOT)
			{
			DrawEdge(hDC, &rc, BDR_RAISEDINNER, BF_RECT);
			}


		if (hImageList!=NULL && iImage>=0)
			{
			int cxIcon, cyIcon;
			ImageList_GetIconSize(hImageList, &cxIcon, &cyIcon);

			if ((uState & THEME_DISABLED)==0)
				{
				ImageList_Draw(hImageList, iImage, hDC, 
						rc.left + CX_BUTTONMARGIN,
						rc.top+ CY_BUTTONMARGIN, ILD_TRANSPARENT);
				}
			else
				{
				HICON hIcon=ImageList_GetIcon(hImageList, iImage, 0);
				DrawState(hDC, NULL, NULL, (LPARAM)hIcon, 0, rc.left + CX_BUTTONMARGIN, rc.top+ CY_BUTTONMARGIN, cxIcon, cyIcon, DST_ICON|DSS_DISABLED);
				DestroyIcon(hIcon);
				}

			rc.left=rc.left + cxIcon + CX_BUTTONMARGIN + CX_ICONTEXTGAP;
			}
		else
			{
			rc.left +=CX_BUTTONMARGIN+CX_TEXTRIGHTMARGIN/2+1;
			}


		if (!IsEmptyString(pszText))
			{
			TEXTMETRIC tm;
			GetTextMetrics(hDC, &tm);

			rc.top=rc.top + (rc.bottom-rc.top-tm.tmHeight)/2;

			if (uState & THEME_DISABLED)
				{
				DrawState(hDC, NULL, NULL, (LPARAM)pszText, 0, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, DST_PREFIXTEXT|DSS_DISABLED);
				}
			else
				{
				SetTextColor(hDC, COLOR_BTNTEXT);
				SlxDrawText(hDC, pszText, -1, &rc, 0);
				}
			}
		}
}


void DrawThemedToolBarButton(HWND hWnd, HDC hDC, LPCRECT prc, LPCWSTR pszText, 
					UINT uState, HIMAGELIST hImageList, int iImage)
{
	CThemeData theme;
	theme.Open(hWnd, L"Toolbar");
	DrawThemedToolBarButton(theme, hDC, prc, pszText, uState, hImageList, iImage);
}


}	// namespace Simple
