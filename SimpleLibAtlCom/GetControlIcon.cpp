//////////////////////////////////////////////////////////////////////
//
// SimpleLib ATL COM Version 1.0
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
// GetControlIcon.cpp - implementation of GetControlIcon

#include "stdafx.h"
#include "SimpleLibAtlComBuild.h"

#include "GetControlIcon.h"
#include <commctrl.h>

namespace Simple
{

HBITMAP SIMPLEAPI GetControlImage(LPCOLESTR pszCLSID, int cx, int cy)
{
	COLORREF cBack = GetSysColor(COLOR_WINDOW);

	CUniString strBitmap;
	if (RegGetString(HKEY_CLASSES_ROOT, Format(L"CLSID\\%s\\ToolboxBitmap32", pszCLSID), NULL, strBitmap)!=ERROR_SUCCESS)
		return NULL;

	// Split into path name and resource id
	CUniString strFile, strResID;
	SplitString(strBitmap, L",", strFile, strResID);
	int nResID=0;
	if (!strResID.IsEmpty())
		nResID=_wtoi(strResID);

	// Load the library
	HINSTANCE hInstLib=LoadLibraryEx(strFile, NULL, LOAD_LIBRARY_AS_DATAFILE);
	if (!hInstLib)
		return NULL;

	// Load image
	HBITMAP hbmpOriginal=(HBITMAP)::LoadImage(hInstLib, MAKEINTRESOURCE(nResID), IMAGE_BITMAP, 0, 0, 0);

	// Free library
	FreeLibrary(hInstLib);

	// Failed?
	if (!hbmpOriginal)
	{
		return NULL;
	}

	// Get size of original bitmap
	BITMAP bmOrig;
	GetObject(hbmpOriginal, sizeof(bmOrig), &bmOrig);
	if (bmOrig.bmWidth==cx && bmOrig.bmHeight==cy)
		return hbmpOriginal;

	// Stretch the image to correct size
	HBITMAP hbmpResized=NULL;
	{
		HDC hdc=CreateIC(_T("display"), NULL, NULL, NULL);
		HDC hdcResized=CreateCompatibleDC(hdc);
		hbmpResized=CreateCompatibleBitmap(hdc, cx, cy);
		HBITMAP hbmpOldResized=(HBITMAP)SelectObject(hdcResized, hbmpResized);
		HDC hdcOrig=CreateCompatibleDC(hdc);
		HBITMAP hbmpOldOrig=(HBITMAP)SelectObject(hdcOrig, hbmpOriginal);

		StretchBlt(hdcResized, 0, 0, cx, cy, hdcOrig, 0, 0, bmOrig.bmWidth, bmOrig.bmHeight, SRCCOPY);

		SelectObject(hdcOrig, hbmpOldOrig);
		DeleteDC(hdcOrig);
		SelectObject(hdcResized, hbmpOldResized);
		DeleteDC(hdcResized);
		DeleteDC(hdc);
	}

	DeleteObject(hbmpOriginal);

	// Resize failed?
	if (!hbmpResized)
	{
		return NULL;
	}

	return hbmpResized;
}

HICON SIMPLEAPI GetControlIcon(LPCOLESTR pszCLSID, int cx, int cy)
{
	HBITMAP hbmp = GetControlImage(pszCLSID, cx, cy);
	if (!hbmp)
		return NULL;

	HIMAGELIST himl;
	himl=ImageList_Create(cx, cy, ILC_COLOR32|ILC_MASK, 0, 1);

	// find out the mask color. this is the 1st pixel in the bitmap's bottom left
	COLORREF cr;
	{
		HDC hdc=CreateIC(_T("display"), NULL, NULL, NULL);
		HDC hdcOrig=CreateCompatibleDC(hdc);
		HBITMAP hbmpOldOrig=(HBITMAP)SelectObject(hdcOrig, hbmp);
		COLORREF crPixelTop    = GetPixel(hdcOrig, 0, 0);
		COLORREF crPixelBottom = GetPixel(hdcOrig, 0, cy-1);
		cr = crPixelBottom;

		if (crPixelBottom!=crPixelTop || crPixelBottom==RGB(0,0,0)) // do some heuristic guess work
		{
			// if pixel top is grey, and bottom left is not green nor magenta (ie common
			if (crPixelBottom!=RGB(255,0,255) && crPixelBottom!=RGB(0,255,0) && crPixelTop==RGB(192,192,192))
				cr = crPixelTop;
			// if pixel bottom is black or white
			else if (crPixelBottom==RGB(0,0,0) || crPixelBottom==RGB(255,255,255))
			{
				if (crPixelTop==RGB(255,0,255) || crPixelTop==RGB(0,255,0)) // and top left is green or magenta 
					cr = crPixelTop;
				else
					cr = RGB(192,192,192); // use grey if not a known color
			}
		}


		SelectObject(hdcOrig, hbmpOldOrig);
		DeleteDC(hdcOrig);
		DeleteDC(hdc);
	}

	ImageList_AddMasked(himl, hbmp, cr);
	HICON hicon = ImageList_GetIcon(himl, 0, 0);
	DeleteObject(hbmp);

	ImageList_Destroy(himl);
	return hicon;
}



}	// namespace Simple
