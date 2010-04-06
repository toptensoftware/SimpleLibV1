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
// GdiMiscEx.h - declaration of miscellaneous GDI functions

#ifndef __GDIMISCEX_H
#define __GDIMISCEX_H

namespace Gdiplus 
{
	class Bitmap;
};

namespace Simple
{

// Various ImageBlt overloads...
// If rgbKey is COLORKEY_NONE for non-transparent blt
// If rgbKey is COLORKEY_ALPHA for AlphaBlend blt
#define COLORKEY_NONE			0xFF000000
#define COLORKEY_ALPHA			0xFE0000FF
#define COLORKEY_ALPHA_TRANS	0xFE000000		// Or with alpha value (255-opaque)
void SIMPLEAPI ImageBlt(HDC hdcDest, int iDestLeft, int iDestTop, int iDestWidth, int iDestHeight, 
				HDC hdcSrc, int iSrcLeft, int iSrcTop, int iSrcWidth, int iSrcHeight, 
				COLORREF rgbKey);
void SIMPLEAPI ImageBlt(HDC hdcDest, RECT* prcDest, HDC hdcSrc, RECT* prcSrc, COLORREF rgbKey);
void SIMPLEAPI ImageBlt(HDC hdcDest, int iDestLeft, int iDestTop, int iDestWidth, int iDestHeight, 
				HBITMAP hbm, int iSrcLeft, int iSrcTop, int iSrcWidth, int iSrcHeight, 
				COLORREF rgbKey);
void SIMPLEAPI ImageBlt(HDC hdcDest, RECT* prcDest, HBITMAP hbm, RECT* prcSrc, COLORREF rgbKey);

// Draws a stretched image but doesn't stretch the edges as defined by "m"
void SIMPLEAPI EdgedStretchBlt(HDC hDC, RECT margins, RECT rcDest, HDC hdcMem, RECT rcSrc, COLORREF rgbKey);
void SIMPLEAPI EdgedStretchBlt(HDC hDC, RECT margins, RECT rcDest, HBITMAP hbmp, RECT rcSrc, COLORREF rgbKey);

// Alpha blend colors
BYTE SIMPLEAPI AlphaBlendColor(BYTE b1, BYTE b2, BYTE bAlpha);
COLORREF SIMPLEAPI AlphaBlendColor(COLORREF rgb1, COLORREF rgb2, BYTE bAlpha);

// Convert a gdi plus bitmap to a Win32 bitmap, preserving alpha channel
HBITMAP SIMPLEAPI BitmapFromGdiplusBitmap(Gdiplus::Bitmap& bmSrc);

// Load a png resource.  Resource Type should be "PNG"
HBITMAP SIMPLEAPI LoadPngImage(HINSTANCE hInstance, const wchar_t* pszResource, bool bPreMultiply=true);

// Load a dib resource.
HBITMAP SIMPLEAPI LoadAlphaDib(HINSTANCE hInstance, const wchar_t* pszResID, bool bPreMultiply=true);

// Calculate various colors for drawing XP style menus
#define XPSTATE_CHECKED	0x0001
#define XPSTATE_HOT		0x0002
COLORREF SIMPLEAPI GetMixedColor(COLORREF colorA,COLORREF colorB);
COLORREF SIMPLEAPI GetMidColor(COLORREF colorA,COLORREF colorB);
COLORREF SIMPLEAPI GetXPHighlightColor(UINT uiState, COLORREF crBg);

// Premultiply an alpha channel dib so it will work with AlphaBlend
void SIMPLEAPI PreMultiplyDib(HBITMAP hDIB);

// Convert a dib to gray scale
void SIMPLEAPI GrayScaleDib(HBITMAP hDIB, COLORREF rgb=RGB(255,255,255), BYTE bBlend=0);

// Blend a dib with a solid color
void SIMPLEAPI AlphaBlendDibWithColor(HBITMAP hDIB, COLORREF rgb, BYTE bBlend);

// Color key a dib, changing all of one color to alpha 0
void SIMPLEAPI ColorKeyDib(HBITMAP hDIB, COLORREF rgbColor);

// Convert a dib to a shadow by making all pixels black but 1/4 of original alpha
void SIMPLEAPI ShadowDib(HBITMAP hDIB);

// Perform a blt using AlphaBlend function
void SIMPLEAPI AlphaBlt(HDC hdcDest, int x, int y, int cx, int cy,
				HDC hdcSrc, int sx, int sy, int scx, int scy, BYTE bAlpha=255);


#ifdef _INC_COMMCTRL
// Enhanced image list loading - selects a 16 or 32 bpp image
HIMAGELIST SIMPLEAPI ImageList_LoadEx(HINSTANCE hInstance, const wchar_t* pszRes4, const wchar_t* pszRes32, COLORREF rgbMask, int cx);

// Load an image list as gray scale
HIMAGELIST SIMPLEAPI ImageList_LoadGrayScale(HINSTANCE hInstance, const wchar_t* pszRes32, int cx, bool bShadow);
#endif


class CRgnData
{
public:
// Construcion
	CRgnData(int iInitialRects=10);
	~CRgnData();

// Operations
	void AddRect(int left, int top, int right, int bottom);
	void ExtendLastRect();
	HRGN Create();

// Attributes
	DWORD		m_iRectsAllocated;
	RGNDATA*	m_pRgnData;
};

HRGN SIMPLEAPI CreateRegionFromAlphaChannel(HBITMAP hDIB, bool bConvex);


}	// namespace Simple


#endif	// __GDIMISCEX_H


