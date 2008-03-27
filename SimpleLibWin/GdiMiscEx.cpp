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
// GdiMiscEx.cpp - implementation of miscellaneous GDI functions

#include "stdafx.h"
#include "SimpleLibWinBuild.h"

#include "GdiMiscEx.h"
#include "GdiText.h"
#include "GeoOps.h"
#include "SmartHandle.h"
#include "SelectObject.h"
#include "SimpleMemoryStream.h"
#include "GdiPlusLoader.h"
#include "GdiMisc.h"
#include "VersionInfo.h"

#pragma comment(lib, "msimg32.lib")

namespace Simple
{

void ImageBlt(HDC hdcDest, int iDestLeft, int iDestTop, int iDestWidth, int iDestHeight, 
				HDC hdcSrc, int iSrcLeft, int iSrcTop, int iSrcWidth, int iSrcHeight, 
				COLORREF rgbKey)
{
	if (rgbKey==COLORKEY_NONE)
	{
		if (iSrcHeight==iSrcWidth && iDestHeight==iSrcHeight)
		{
			BitBlt(hdcDest, iDestLeft, iDestTop, iDestWidth, iDestHeight, hdcSrc, iSrcLeft, iSrcTop, SRCCOPY);
		}
		else
		{
			StretchBlt(hdcDest, iDestLeft, iDestTop, iDestWidth, iDestHeight, hdcSrc, iSrcLeft, iSrcTop, iSrcWidth, iSrcHeight, SRCCOPY);
		}
	}
	else if ((rgbKey&0xFF000000)==COLORKEY_ALPHA_TRANS)
	{
		AlphaBlt(hdcDest, iDestLeft, iDestTop, iDestWidth, iDestHeight, hdcSrc, iSrcLeft, iSrcTop, iSrcWidth, iSrcHeight, LOBYTE(LOWORD(rgbKey)));
	}
	else
	{
		TransparentBlt(hdcDest, iDestLeft, iDestTop, iDestWidth, iDestHeight, hdcSrc, iSrcLeft, iSrcTop, iSrcWidth, iSrcHeight, rgbKey);
	}
}

void ImageBlt(HDC hdcDest, RECT* prcDest, HDC hdcSrc, RECT* prcSrc, COLORREF rgbKey)
{
	ImageBlt(hdcDest, prcDest->left, prcDest->top, Width(*prcDest), Height(*prcDest),
				hdcSrc, prcSrc->left, prcSrc->top, Width(*prcSrc), Height(*prcSrc), rgbKey);
}

void ImageBlt(HDC hdcDest, int iDestLeft, int iDestTop, int iDestWidth, int iDestHeight, 
				HBITMAP hbm, int iSrcLeft, int iSrcTop, int iSrcWidth, int iSrcHeight, 
				COLORREF rgbKey)
{
	HDC hdcMem=CreateCompatibleDC(hdcDest);

	{
	CSelectObject hOldBitmap(hdcMem, hbm);
	ImageBlt(hdcDest, iDestLeft, iDestTop, iDestWidth, iDestHeight, 
				hdcMem, iSrcLeft, iSrcTop, iSrcWidth, iSrcHeight, rgbKey);
	}
	DeleteDC(hdcMem);
}


void ImageBlt(HDC hdcDest, RECT* prcDest, HBITMAP hbm, RECT* prcSrc, COLORREF rgbKey)
{
	HDC hdcMem=CreateCompatibleDC(hdcDest);

	{
	CSelectObject hOldBitmap(hdcMem, hbm);
	ImageBlt(hdcDest, prcDest, hdcMem, prcSrc, rgbKey);
	}
	DeleteDC(hdcMem);
}


void EdgedStretchBlt(HDC hDC, RECT m, RECT rcDest, HDC hdcMem, RECT rcSrc, COLORREF rgbKey)
{
	// Redundant stretch
	if (Width(rcDest)==Width(rcSrc) && Height(rcDest)==Height(rcSrc))
	{
		ImageBlt(hDC, &rcDest, hdcMem, &rcSrc, rgbKey);
		return;
	}

	// Check for shrink to smaller than margin edges - and just do a normal stretch
	if (Height(rcDest)<(m.top+m.bottom))
	{
		m.top=0;
		m.bottom=0;
	}
	if (Width(rcDest)<(m.left+m.right))
	{
		m.left=0;
		m.right=0;
	}

	bool h=m.left || m.right;
	bool v=m.top || m.bottom;

	if (!v && !h)
		{
		// No special stretching
		ImageBlt(hDC, &rcDest, hdcMem, &rcSrc, rgbKey);
		return;
		}

	// Special horizontal stretch 
	if (h && !v)
		{
		// Left
		if (m.left)
			{
			ImageBlt(hDC, rcDest.left, rcDest.top, m.left, Height(rcDest), 
						hdcMem, rcSrc.left, rcSrc.top, m.left, Height(rcSrc), rgbKey);
			}

		// Middle
		ImageBlt(hDC, rcDest.left + m.left, rcDest.top, Width(rcDest)-m.left-m.right, Height(rcDest),
				 hdcMem, rcSrc.left + m.left, rcSrc.top, Width(rcSrc)-m.left-m.right, Height(rcSrc), rgbKey);

		// Right
		if (m.right)
			{
			ImageBlt(hDC, rcDest.right-m.right, rcDest.top, m.right, Height(rcDest),
						hdcMem, rcSrc.right-m.right, rcSrc.top, m.right, Height(rcSrc), rgbKey);
			}

		return;
		}

	// Special vertical stretch 
	if (v && !h)
		{
		// Top
		if (m.top)
			{
			ImageBlt(hDC, rcDest.left, rcDest.top, Width(rcDest), m.top,
						hdcMem, rcSrc.left, rcSrc.top, Width(rcSrc), m.top, rgbKey);
			}

		// Middle
		ImageBlt(hDC, rcDest.left, rcDest.top + m.top, Width(rcDest), Height(rcDest)-m.top-m.bottom,
					hdcMem, rcSrc.left, rcSrc.top + m.top, Width(rcSrc), Height(rcSrc)-m.top-m.bottom, rgbKey);

		// Bottom
		if (m.bottom)
			{
			ImageBlt(hDC, rcDest.left, rcDest.bottom-m.bottom, Width(rcDest), m.bottom,
						hdcMem, rcSrc.left, rcSrc.bottom-m.bottom, Width(rcSrc), m.bottom, rgbKey);
			}

		return;
		}


	RECT rcM=m;
	rcM.top=0;
	rcM.bottom=0;

	if (m.top)
		{
		RECT rcDest2=rcDest;
		RECT rcSrc2=rcSrc;
		rcDest2.bottom=rcDest2.top+m.top;
		rcSrc2.bottom=rcSrc2.top+m.top;

		EdgedStretchBlt(hDC, rcM, rcDest2, hdcMem, rcSrc2, rgbKey);
		}

	RECT rcDest2=rcDest;
	rcDest2.top+=m.top;
	rcDest2.bottom-=m.bottom;

	RECT rcSrc2=rcSrc;
	rcSrc2.top+=m.top;
	rcSrc2.bottom-=m.bottom;

	EdgedStretchBlt(hDC, rcM, rcDest2, hdcMem, rcSrc2, rgbKey);

	if (m.bottom)
		{
		RECT rcDest2=rcDest;
		rcDest2.top=rcDest2.bottom-m.bottom;

		RECT rcSrc2=rcSrc;
		rcSrc2.top=rcSrc2.bottom-m.bottom;

		EdgedStretchBlt(hDC, rcM, rcDest2, hdcMem, rcSrc2, rgbKey);
		}
}


void EdgedStretchBlt(HDC hDC, RECT m, RECT rcDest, HBITMAP hbmp, RECT rcSrc, COLORREF rgbKey)
{
	HDC hdcMem=CreateCompatibleDC(hDC);
	{
	if (IsRectEmpty(&rcSrc))
		{
		BITMAP bm;
		GetObject(hbmp, sizeof(BITMAP), &bm);
		rcSrc=SRect(0,0,bm.bmWidth,bm.bmHeight);
		}

	CSelectObject hOldBitmap(hdcMem, hbmp);
	EdgedStretchBlt(hDC, m, rcDest, hdcMem, rcSrc, rgbKey);
	}
	DeleteDC(hdcMem);
}

BYTE AlphaBlendColor(BYTE b1, BYTE b2, BYTE bAlpha)
{
	int iVal=b2-b1;
	iVal = (iVal * bAlpha) / 255;
	return (BYTE)(b1+iVal);
}

COLORREF AlphaBlendColor(COLORREF rgb1, COLORREF rgb2, BYTE bAlpha)
{
	return RGB(
				AlphaBlendColor(GetRValue(rgb1), GetRValue(rgb2), bAlpha),
				AlphaBlendColor(GetGValue(rgb1), GetGValue(rgb2), bAlpha),
				AlphaBlendColor(GetBValue(rgb1), GetBValue(rgb2), bAlpha) );
}

#include <gdiplus.h>

HBITMAP BitmapFromGdiplusBitmap(Gdiplus::Bitmap& bmSrc)
{
	// Get source pixel format
	Gdiplus::PixelFormat pfSrc=bmSrc.GetPixelFormat();

	// Work out destination pixel format
	UINT nBPP = 32;
	Gdiplus::PixelFormat pfDest=PixelFormat32bppRGB;
	if (pfSrc & PixelFormatGDI)
	{
		nBPP = Gdiplus::GetPixelFormatSize( pfSrc );
		pfDest = pfSrc;
	}

	// If source has alpha channel, enfore 32 bit ARGB
	if (Gdiplus::IsAlphaPixelFormat(pfSrc))
	{
		nBPP = 32;
		pfDest = PixelFormat32bppARGB;
	}

	// Setup bitmap info
	LPBITMAPINFO pbmi=(LPBITMAPINFO)_alloca(sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*256);
	memset( &pbmi->bmiHeader, 0, sizeof( pbmi->bmiHeader ) );
	pbmi->bmiHeader.biSize = sizeof( pbmi->bmiHeader );
	pbmi->bmiHeader.biWidth = bmSrc.GetWidth();
	pbmi->bmiHeader.biHeight = bmSrc.GetHeight();
	pbmi->bmiHeader.biPlanes = 1;
	pbmi->bmiHeader.biBitCount = nBPP;
	pbmi->bmiHeader.biCompression = BI_RGB;
	if (nBPP<=8)
	{
		memset( pbmi->bmiColors, 0, 256*sizeof( RGBQUAD ) );
	}

	// Create the bitmap
	void* pBits;
	HBITMAP hBitmap=CreateDIBSection(NULL, pbmi, DIB_RGB_COLORS, &pBits, NULL, 0);
	if (!hBitmap)
		return NULL;

	// If indexed pixel format, setup color palette
	Gdiplus::ColorPalette* pPalette = NULL;
	if(Gdiplus::IsIndexedPixelFormat(pfSrc))
	{
		// Get source color palette
		UINT nPaletteSize = bmSrc.GetPaletteSize();
		pPalette = static_cast<Gdiplus::ColorPalette*>(_alloca(nPaletteSize));
		bmSrc.GetPalette( pPalette, nPaletteSize );

		// Convert RGB valuesformat
		RGBQUAD argbPalette[256];
		for (UINT iColor = 0; iColor < pPalette->Count; iColor++)
		{
			Gdiplus::ARGB color = pPalette->Entries[iColor];
			argbPalette[iColor].rgbRed = (BYTE)( (color>>RED_SHIFT) & 0xff );
			argbPalette[iColor].rgbGreen = (BYTE)( (color>>GREEN_SHIFT) & 0xff );
			argbPalette[iColor].rgbBlue = (BYTE)( (color>>BLUE_SHIFT) & 0xff );
			argbPalette[iColor].rgbReserved = 0;
		}

		// Apply the color paletter to the destination image
		HDC hDC=CreateCompatibleDC(NULL);
		HBITMAP hbmpOld=(HBITMAP)SelectObject(hDC, hBitmap);
		::SetDIBColorTable(hDC, 0, pPalette->Count, argbPalette);
		SelectObject(hDC, hbmpOld);
		DeleteDC(hDC);
	}

	// Work out pitch of destination image
	int iPitch=(((bmSrc.GetWidth()*nBPP)+31)/32)*4;

	// Save pixel format
	if (pfDest==pfSrc)
	{
		// Same pixel format, just copy bits

		// DIB sections are upside down so adjust buffer starting position and pitch
		pBits = LPBYTE( pBits )+((bmSrc.GetHeight()-1)*iPitch);
		iPitch = -iPitch;

		// Lock source buffer
		Gdiplus::BitmapData data;
		Gdiplus::Rect rect(0, 0, bmSrc.GetWidth(), bmSrc.GetHeight());
		if(bmSrc.LockBits( &rect, Gdiplus::ImageLockModeRead, pfSrc, &data )!=Gdiplus::Ok)
		{
			DeleteObject(hBitmap);
			return NULL;
		}

		// Work out byte per row by rounding up
		int iBytesPerRow=((nBPP*bmSrc.GetWidth()+7) & ~7)/8;

		// Get row pointers
		BYTE* pbDestRow=(BYTE*)pBits;
		BYTE* pbSrcRow=(BYTE*)data.Scan0;
		for (int y=0; y<int(bmSrc.GetHeight()); y++)
		{
			// Copy it
			memcpy(pbDestRow, pbSrcRow, iBytesPerRow);

			// Update pointers
			pbDestRow+=iPitch;
			pbSrcRow+=data.Stride;
		}

		// Release source bits
		bmSrc.UnlockBits( &data );
	}
	else
	{
		// Use GDI+ to do the conversion
		Gdiplus::Bitmap bmDest( bmSrc.GetWidth(), bmSrc.GetHeight(), iPitch, pfDest, (BYTE*)pBits);
		Gdiplus::Graphics g(&bmDest);
		g.DrawImage( &bmSrc, 0, 0 );
	}

	// Done!
	return hBitmap;
}

// Load a png image from a resource
HBITMAP LoadPngImage(HINSTANCE hInstance, const wchar_t* pszResource, bool bPreMultiply)
{
	// Open the resource stream
	CAutoPtr<IStream, SRefCounted> spStream;
	if (FAILED(OpenResourceStream(hInstance, pszResource, L"PNG", &spStream)))
		return NULL;

	// Load GDI plus if not already loaded
	CGdiPlusLoader gdip;

	// Load bitmp
	Gdiplus::Bitmap bmSrc(spStream);
	if( bmSrc.GetLastStatus() != Gdiplus::Ok)
		return NULL;

	// Convert to bitmap
	HBITMAP hBitmap=BitmapFromGdiplusBitmap(bmSrc);
	if (!hBitmap)
		return NULL;

	// Premultiply it  so its ready for use with AlphaBlend
	if (bPreMultiply)
		PreMultiplyDib(hBitmap);

	return hBitmap;
}

COLORREF GetMixedColor(COLORREF colorA,COLORREF colorB)
{
	// ( 86a + 14b ) / 100
	int red   = MulDiv(86,GetRValue(colorA),100) + MulDiv(14,GetRValue(colorB),100);
	int green = MulDiv(86,GetGValue(colorA),100) + MulDiv(14,GetGValue(colorB),100);
	int blue  = MulDiv(86,GetBValue(colorA),100) + MulDiv(14,GetBValue(colorB),100);

	return RGB( red,green,blue);
}

COLORREF GetMidColor(COLORREF colorA,COLORREF colorB)
{ 
	// (7a + 3b)/10
	int red   = MulDiv(7,GetRValue(colorA),10) + MulDiv(3,GetRValue(colorB),10);
	int green = MulDiv(7,GetGValue(colorA),10) + MulDiv(3,GetGValue(colorB),10);
	int blue  = MulDiv(7,GetBValue(colorA),10) + MulDiv(3,GetBValue(colorB),10);

	return RGB( red, green, blue);
}

COLORREF GetXPHighlightColor(UINT uiState, COLORREF crBg) 
{
	ASSERT(uiState!=0);
	BYTE bGamma, bAlpha;
	if ((uiState & (XPSTATE_HOT|XPSTATE_CHECKED))==(XPSTATE_HOT|XPSTATE_CHECKED))
	{
		bAlpha = 196;
		bGamma = 64;
	}
	else
	if (uiState & XPSTATE_HOT)
	{
		bAlpha = 160;
		bGamma = 127;
	}
	else
	{
		// checked
		bAlpha = 128;
		bGamma = 196;
	}


	COLORREF crHilight = GetSysColor(COLOR_HIGHLIGHT);
	COLORREF crGamma   = AlphaBlendColor( crHilight, RGB(255,255,255), bGamma);
	COLORREF crInnerHilight = AlphaBlendColor( crBg, crGamma, bAlpha);
	return crInnerHilight;
}


#define GetAValue32(rgb)      ((BYTE)((rgb)>>24))
#define GetBValue32(rgb)      (LOBYTE(rgb))
#define GetGValue32(rgb)      (LOBYTE(((WORD)(rgb)) >> 8))
#define GetRValue32(rgb)      (LOBYTE((rgb)>>16))
#define RGBA(r,g,b,a)        ((COLORREF)(((BYTE)(b)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(r))<<16))|(((DWORD)(BYTE)(a))<<24))


void PreMultiplyDib(HBITMAP hDIB)
{
	DIBSECTION dib;
	if (!GetObject(hDIB, sizeof(dib), &dib))
		return;

	DWORD* pdw=(DWORD*)dib.dsBm.bmBits;
	
	ASSERT((dib.dsBm.bmWidthBytes%4)==0);

	int iPixels=dib.dsBm.bmHeight * dib.dsBm.bmWidthBytes/4;
	for (int i=0; i<iPixels; i++)
		{
		BYTE a=GetAValue32(*pdw);
		BYTE r=GetRValue32(*pdw) * a / 255;
		BYTE g=GetGValue32(*pdw) * a / 255;
		BYTE b=GetBValue32(*pdw) * a / 255;

		*pdw=RGBA(r, g, b, GetAValue32(*pdw));
		pdw++;
		}

}

// Convert a DIB to grayscale
void GrayScaleDib(HBITMAP hDIB, COLORREF rgb, BYTE bBlend)
{
	DIBSECTION dib;
	GetObject(hDIB, sizeof(dib), &dib);

	DWORD* pdw=(DWORD*)dib.dsBm.bmBits;
	
	ASSERT((dib.dsBm.bmWidthBytes%4)==0);

	int iPixels=dib.dsBm.bmHeight * dib.dsBm.bmWidthBytes/4;
	for (int i=0; i<iPixels; i++)
	{
		BYTE a=GetAValue32(*pdw);
		BYTE r=GetRValue32(*pdw);
		BYTE g=GetGValue32(*pdw);
		BYTE b=GetBValue32(*pdw);

		int l=(int)(0.3*r + 0.59*g + 0.11 * b);
		l=AlphaBlendColor((BYTE)l, (BYTE)255, bBlend);

		l=l*a/255;

		*pdw=RGBA(l * GetRValue(rgb)/255, 
					l * GetGValue(rgb)/255, 
					l * GetBValue(rgb)/255, a);
		pdw++;
	}

}

// Convert a DIB to grayscale
void ColorKeyDib(HBITMAP hDIB, COLORREF rgbColor)
{
	rgbColor|=0xFF000000;

	DIBSECTION dib;
	GetObject(hDIB, sizeof(dib), &dib);

	DWORD* pdw=(DWORD*)dib.dsBm.bmBits;
	
	ASSERT((dib.dsBm.bmWidthBytes%4)==0);

	int iPixels=dib.dsBm.bmHeight * dib.dsBm.bmWidthBytes/4;
	for (int i=0; i<iPixels; i++)
	{
		if (*pdw==rgbColor)
		{
			*pdw=0;
		}
		pdw++;
	}

}
// Convert a dib to a shadow by making all pixels black but 1/4 of original alpha
void ShadowDib(HBITMAP hDIB)
{
	DIBSECTION dib;
	GetObject(hDIB, sizeof(dib), &dib);

	DWORD* pdw=(DWORD*)dib.dsBm.bmBits;
	
	ASSERT((dib.dsBm.bmWidthBytes%4)==0);

	int iPixels=dib.dsBm.bmHeight * dib.dsBm.bmWidthBytes/4;
	for (int i=0; i<iPixels; i++)
	{
		BYTE a=GetAValue32(*pdw);
		*pdw=RGBA(0, 0, 0, a/4);
		pdw++;
	}

}

HBITMAP LoadAlphaDib(HINSTANCE hInstance, const wchar_t* pszResID, bool bPreMultiply)
{
	HBITMAP hDIB=(HBITMAP)LoadImage(hInstance, pszResID, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	if (!hDIB)
		return NULL;

	if (bPreMultiply)
		PreMultiplyDib(hDIB);

	return hDIB;
}

void AlphaBlt(HDC hdcDest, int x, int y, int cx, int cy,
				HDC hdcSrc, int sx, int sy, int scx, int scy, BYTE bAlpha)
{
	BLENDFUNCTION b;
	b.AlphaFormat=AC_SRC_ALPHA;
	b.BlendOp=AC_SRC_OVER;
	b.BlendFlags=0;
	b.SourceConstantAlpha=bAlpha;

	AlphaBlend(hdcDest, x, y, cx, cy, hdcSrc, sx, sy, scx, scy, b);
}


HIMAGELIST ImageList_LoadEx(HINSTANCE hInstance, const wchar_t* pszRes4, const wchar_t* pszRes32, COLORREF rgbMask, int cx)
{
	// Limit to 16 color
	CSmartHandle<HDC> hdcMem=CreateIC(L"display", NULL, NULL, NULL);
	if (GetDeviceCaps(hdcMem, BITSPIXEL)<16 || !pszRes32)
	{
		return ImageList_LoadImage(hInstance, pszRes4, cx, 1, rgbMask, IMAGE_BITMAP, LR_CREATEDIBSECTION);
	}

	// If common controls 6, just load alpha image directly into image list
	if (HIWORD(GetCommonControlsVersion())>=6)
	{
		// Load high color bitmap...
		CSmartHandle<HBITMAP> hBitmap=(HBITMAP)LoadImage(hInstance, pszRes32, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
		if (!hBitmap)
			return NULL;
		
		// Get its size
		BITMAP bm;
		GetObject(hBitmap, sizeof(bm), &bm);

		// Create image list
		HIMAGELIST hImageList=ImageList_Create(cx, bm.bmHeight, ILC_COLOR32 | ILC_MASK, bm.bmWidth/cx, 1);
		if (!hImageList)
			return NULL;

		// Add image...
		ImageList_Add(hImageList, hBitmap, NULL);

		return hImageList;
	}
	else
	{
		// Load alpha blended image and premultiply it...
		CSmartHandle<HBITMAP> hBitmap=LoadAlphaDib(hInstance, pszRes32);
		if (!hBitmap)
			return NULL;
		
		// Get its size
		BITMAP bm;
		GetObject(hBitmap, sizeof(bm), &bm);

		CSmartHandle<HDC> hdcScreen=CreateIC(L"display", NULL, NULL, NULL);
		CSmartHandle<HBITMAP> hBitmap2=CreateCompatibleBitmap(hdcScreen, bm.bmWidth, bm.bmHeight);

		{
			CSmartHandle<HDC> hdcMem=CreateCompatibleDC(hdcScreen);
			CSmartHandle<HDC> hdcMem2=CreateCompatibleDC(hdcScreen);
			CSelectObject hOldBitmap(hdcMem, hBitmap);
			CSelectObject hOldBitmap2(hdcMem2, hBitmap2);

			FillRect(hdcMem2, 0, 0, bm.bmWidth, bm.bmHeight, GetSysColorBrush(COLOR_BTNFACE));
			AlphaBlt(hdcMem2, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, bm.bmWidth, bm.bmHeight);
		}

		// Create image list
		HIMAGELIST hImageList=ImageList_Create(cx, bm.bmHeight, ILC_COLOR32 | ILC_MASK, bm.bmWidth/cx, 1);
		if (!hImageList)
			return NULL;

		// Add image...
		ImageList_AddMasked(hImageList, hBitmap2, GetSysColor(COLOR_BTNFACE));

		return hImageList;
	}
}

HIMAGELIST ImageList_LoadGrayScale(HINSTANCE hInstance, const wchar_t* pszRes32, int cx, bool bShadow)
{
	// If limited to 16, no grayscale
	CSmartHandle<HDC> hdcMem=CreateIC(L"display", NULL, NULL, NULL);
	if (GetDeviceCaps(hdcMem, BITSPIXEL)<16 || !pszRes32)
	{
		return NULL;
	}

	// Load image
	CSmartHandle<HBITMAP> hBitmap=(HBITMAP)LoadImage(hInstance, pszRes32, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	if (!hBitmap)
		return NULL;

	// Convert to gray scale
	if (bShadow)
		ShadowDib(hBitmap);
	else
		GrayScaleDib(hBitmap);

	// Get its size
	BITMAP bm;
	GetObject(hBitmap, sizeof(bm), &bm);

	// If common controls 6, just load alpha image directly into image list
	if (HIWORD(GetCommonControlsVersion())>=6)
	{
		// Create image list
		HIMAGELIST hImageList=ImageList_Create(cx, bm.bmHeight, ILC_COLOR32 | ILC_MASK, bm.bmWidth/cx, 1);
		if (!hImageList)
			return NULL;

		// Add image...
		ImageList_Add(hImageList, hBitmap, NULL);

		return hImageList;
	}
	else
	{
		CSmartHandle<HDC> hdcScreen=CreateIC(L"display", NULL, NULL, NULL);
		CSmartHandle<HBITMAP> hBitmap2=CreateCompatibleBitmap(hdcScreen, bm.bmWidth, bm.bmHeight);

		{
			CSmartHandle<HDC> hdcMem=CreateCompatibleDC(hdcScreen);
			CSmartHandle<HDC> hdcMem2=CreateCompatibleDC(hdcScreen);
			CSelectObject hOldBitmap(hdcMem, hBitmap);
			CSelectObject hOldBitmap2(hdcMem2, hBitmap2);

			FillRect(hdcMem2, 0, 0, bm.bmWidth, bm.bmHeight, GetSysColorBrush(COLOR_BTNFACE));
			AlphaBlt(hdcMem2, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, bm.bmWidth, bm.bmHeight);
		}

		// Create image list
		HIMAGELIST hImageList=ImageList_Create(cx, bm.bmHeight, ILC_COLOR32 | ILC_MASK, bm.bmWidth/cx, 1);
		if (!hImageList)
			return NULL;

		// Add image...
		ImageList_AddMasked(hImageList, hBitmap2, GetSysColor(COLOR_BTNFACE));

		return hImageList;
	}
}

// Constructor
CRgnData::CRgnData(int iInitialRects)
{
	m_iRectsAllocated=iInitialRects;

	// Setup RGNDATA
	m_pRgnData=(RGNDATA*)malloc(sizeof(RGNDATAHEADER) + sizeof(RECT) * m_iRectsAllocated);
	m_pRgnData->rdh.dwSize=sizeof(RGNDATAHEADER);
	m_pRgnData->rdh.iType=RDH_RECTANGLES;
	m_pRgnData->rdh.nCount=0;
	m_pRgnData->rdh.rcBound=SRect(0,0,0,0);
}

// Destructor
CRgnData::~CRgnData()
{
	// Free temporary memory
	free(m_pRgnData);
}

// AddRect
void CRgnData::AddRect(int left, int top, int right, int bottom)
{
	// Buffer big enough?
	if (m_pRgnData->rdh.nCount+1 > m_iRectsAllocated)
		{
		// No, reallocate
		m_iRectsAllocated*=2;
		m_pRgnData=(RGNDATA*)realloc(m_pRgnData, sizeof(RGNDATAHEADER) + sizeof(RECT) * m_iRectsAllocated);
		}

	// Get rectangle buffer
	RECT& rcScan=((RECT*)m_pRgnData->Buffer)[m_pRgnData->rdh.nCount];
	rcScan.left=left;
	rcScan.top=top;
	rcScan.right=right;
	rcScan.bottom=bottom;

	// Bump count
	m_pRgnData->rdh.nCount++;

	// Update bounds...
	UnionRect(&m_pRgnData->rdh.rcBound, &m_pRgnData->rdh.rcBound, &rcScan);
}

// Extend last rect
void CRgnData::ExtendLastRect()
{
	ASSERT(m_pRgnData->rdh.nCount>0);
	
	RECT& rcScan=((RECT*)m_pRgnData->Buffer)[m_pRgnData->rdh.nCount-1];
	rcScan.bottom++;

	// Update bounds...
	UnionRect(&m_pRgnData->rdh.rcBound, &m_pRgnData->rdh.rcBound, &rcScan);
}

// Create HRGN
HRGN CRgnData::Create()
{
	// Setup region data size
	m_pRgnData->rdh.nRgnSize=sizeof(RECT) * m_pRgnData->rdh.nCount;

	// Create the region
	return ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + m_pRgnData->rdh.nRgnSize, m_pRgnData);
}





// Convert a DIB to grayscale
HRGN CreateRegionFromAlphaChannel(HBITMAP hDIB, bool bConvex)
{
	if (!hDIB)
		return NULL;

	// Get DIB section info
	DIBSECTION dib;
	GetObject(hDIB, sizeof(dib), &dib);

	// Check its an alpha channel dib
	ASSERT(dib.dsBmih.biBitCount==32);
	ASSERT((dib.dsBm.bmWidthBytes%4)==0);

	// Get bits and width in pixels
	DWORD* pdwBits=(DWORD*)dib.dsBm.bmBits;
	DWORD dwWidthPixels=dib.dsBm.bmWidthBytes/4;

	CRgnData data(10);

	// Previous left/right bounds
	int iPrevL=-100;
	int iPrevR=-100;

	// Scan each row...
	for (int y=0; y<dib.dsBm.bmHeight; y++)
	{
		// Get row pointer
		DWORD* pdwRow=pdwBits + dwWidthPixels*(dib.dsBm.bmHeight-y-1);

		int l=0;
		int r=0;

		// Scan left to right
		for (l=0; l<dib.dsBm.bmWidth; l++)
		{
			if (GetAValue32(pdwRow[l])!=0)
				break;
		}

		// Entire row blank?
		if (l>=dib.dsBm.bmWidth)
		{
			iPrevL=-100;
			iPrevR=-100;
			continue;
		}

		// Scan right to left
		for (r=dib.dsBm.bmWidth; r>0; r--)
		{
			if (GetAValue32(pdwRow[r-1])!=0)
				break;
		}

		if (!bConvex)
		{
			// Scan internal area for holes
			int iStart=l;
			for (int i=l; i<r; i++)
			{
				if (GetAValue32(pdwRow[i])==0)
				{
					// Found a hole, need to get fancy...
					l=-1;
					iPrevL=-100;
					iPrevR=-100;

					data.AddRect(iStart, y, i, y+1);

					// Find next non-transparent
					while (i<r && GetAValue32(pdwRow[i])==0)
						i++;

					iStart=i;
				}
			}

			if (l<0)
			{
				data.AddRect(iStart, y, r, y+1);
				continue;
			}

		}

		// Same as previous row?
		if (iPrevL==l && iPrevR==r)
		{
			data.ExtendLastRect();
			continue;
		}

		data.AddRect(l, y, r, y+1);

		iPrevL=l;
		iPrevR=r;
	}


	// Done!
	return data.Create();
}



}	// namespace Simple
