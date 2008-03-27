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
// GdiMisc.h - declaration of miscellaneous GDI functions

#ifndef __GDIMISC_H
#define __GDIMISC_H

namespace Simple
{
// Create a 1 pixel dither brush
HBRUSH CreatePatternBrush(LPBYTE pBits);
HBRUSH CreateDitherBrush();

// Various FillRect overloads
void FillRect(HDC hDC, int l, int t, int r, int b, HBRUSH hbr);
void FillHLine(HDC hDC, int l, int r, int y, HBRUSH hbr);
void FillVLine(HDC hDC, int t, int b, int x, HBRUSH hbr);

// Xor horizontal/vertical linke
void XorVLine(HDC hDC, int t, int b, int x);
void XorHLine(HDC hDC, int l, int r, int y);

// Calculate the difference between two rectangle (if can)
// Returns false if can't
BOOL DifferenceRect(RECT* prcDest, RECT* prcA, RECT* prcB);

void AlphaFill(HDC hDC, RECT* prc, COLORREF rgb, BYTE bAlpha);

}	// namespace Simple


#endif	// __GDIMISC_H


