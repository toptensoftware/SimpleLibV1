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
HBRUSH SIMPLEAPI CreatePatternBrush(LPBYTE pBits);
HBRUSH SIMPLEAPI CreateDitherBrush();

// Various FillRect overloads
void SIMPLEAPI FillRect(HDC hDC, int l, int t, int r, int b, HBRUSH hbr);
void SIMPLEAPI FillHLine(HDC hDC, int l, int r, int y, HBRUSH hbr);
void SIMPLEAPI FillVLine(HDC hDC, int t, int b, int x, HBRUSH hbr);

// Xor horizontal/vertical linke
void SIMPLEAPI XorVLine(HDC hDC, int t, int b, int x);
void SIMPLEAPI XorHLine(HDC hDC, int l, int r, int y);

// Calculate the difference between two rectangle (if can)
// Returns false if can't
BOOL SIMPLEAPI DifferenceRect(RECT* prcDest, RECT* prcA, RECT* prcB);

void SIMPLEAPI AlphaFill(HDC hDC, RECT* prc, COLORREF rgb, BYTE bAlpha);

}	// namespace Simple


#endif	// __GDIMISC_H


