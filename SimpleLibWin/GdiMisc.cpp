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
// GdiMisc.cpp - implementation of miscellaneous GDI functions

#include "stdafx.h"
#include "SimpleLibWinBuild.h"

#include "GdiMisc.h"
#include "SmartHandle.h"
#include "SelectObject.h"
#include "GeoOps.h"

#pragma comment(lib, "msimg32.lib")

namespace Simple
{

HBRUSH SIMPLEAPI CreatePatternBrush(LPBYTE pBits)
{
	// Create bitmap
	HBITMAP hbm=CreateBitmap(8, 8, 1, 1, pBits);

	// Create dither brush
	LOGBRUSH lb;
	lb.lbStyle=BS_PATTERN;
	lb.lbHatch=(ULONG_PTR)hbm;
	HBRUSH hbr=CreateBrushIndirect(&lb);
	DeleteObject(hbm);

	// Done
	return hbr;
}


HBRUSH SIMPLEAPI CreateDitherBrush()
{
	BYTE bDither[] = {	0x55, 0x55, 0xAA, 0xAA, 0x55, 0x55, 0xAA, 0xAA, 
						0x55, 0x55, 0xAA, 0xAA, 0x55, 0x55, 0xAA, 0xAA, };
	return CreatePatternBrush(bDither);
}

void SIMPLEAPI FillRect(HDC hDC, int l, int t, int r, int b, HBRUSH hbr)
{
	RECT rc;
	SetRect(&rc, l, t, r, b);
	::FillRect(hDC, &rc, hbr);
}

void SIMPLEAPI FillHLine(HDC hDC, int l, int r, int y, HBRUSH hbr)
{
	RECT rc;
	SetRect(&rc, l, y, r, y+1);
	::FillRect(hDC, &rc, hbr);
}

void SIMPLEAPI FillVLine(HDC hDC, int t, int b, int x, HBRUSH hbr)
{
	RECT rc;
	SetRect(&rc, x, t, x+1, b);
	::FillRect(hDC, &rc, hbr);
}


void SIMPLEAPI XorVLine(HDC hDC, int t, int b, int x)
{
	int iOldROP=SetROP2(hDC, R2_NOT);
	MoveToEx(hDC, x, t, NULL);
	LineTo(hDC, x, b);
	//FillVLine(hDC, t, b, x, (HBRUSH)GetStockObject(BLACK_BRUSH));
	SetROP2(hDC, iOldROP);
}

void SIMPLEAPI XorHLine(HDC hDC, int l, int r, int y)
{
	int iOldROP=SetROP2(hDC, R2_NOT);
	MoveToEx(hDC, l, y, NULL);
	LineTo(hDC, r, y);
	//FillVLine(hDC, t, b, x, (HBRUSH)GetStockObject(BLACK_BRUSH));
	SetROP2(hDC, iOldROP);
}

BOOL SIMPLEAPI DifferenceRect(RECT* prcDest, RECT* prcA, RECT* prcB)
{
	// Top/bottom same?
	if (prcA->top==prcB->top && prcA->bottom==prcB->bottom)
		{
		// Left same
		if (prcA->left==prcB->left)
			{
			if (prcA->right==prcB->right)
				{
				// Identical rectangles!
				SetRectEmpty(prcDest);
				return TRUE;
				}

			if (prcA->right > prcB->right)
				{
				// Right extended
				*prcDest=*prcA;
				prcDest->left=prcB->right-1;
				return TRUE;
				}
			else
				{
				// Right shortened
				*prcDest=*prcB;
				prcDest->left=prcA->right-1;
				return TRUE;
				}
			}
		else if (prcA->right==prcB->right)
			{
			if (prcA->left < prcB->left)
				{
				// Left extended
				*prcDest=*prcA;
				prcDest->right=prcB->left+1;
				return TRUE;
				}
			else
				{
				// Left shortened
				*prcDest=*prcB;
				prcDest->right=prcA->left+1;
				return TRUE;
				}
			}
		}

	if (prcA->left==prcB->left && prcA->right==prcB->right)
		{
		// top same
		if (prcA->top==prcB->top)
			{
			if (prcA->bottom > prcB->bottom)
				{
				// bottom extended
				*prcDest=*prcA;
				prcDest->top=prcB->bottom-1;
				return TRUE;
				}
			else
				{
				// bottom shortened
				*prcDest=*prcB;
				prcDest->top=prcA->bottom-1;
				return TRUE;
				}
			}
		else if (prcA->bottom==prcB->bottom)
			{
			if (prcA->top < prcB->top)
				{
				// top extended
				*prcDest=*prcA;
				prcDest->bottom=prcB->top+1;
				return TRUE;
				}
			else
				{
				// top shortened
				*prcDest=*prcB;
				prcDest->bottom=prcA->top+1;
				return TRUE;
				}
			}
		}

	return FALSE;
}





void SIMPLEAPI AlphaFill(HDC hDC, RECT* prc, COLORREF rgb, BYTE bAlpha)
{
	HDC hdcMem=CreateCompatibleDC(hDC);
	{
	CSmartHandle<HBITMAP> hBitmap=CreateCompatibleBitmap(hDC, Width(*prc), Height(*prc));
	CSelectObject hOldBitmap(hdcMem, hBitmap);
	RECT rcFill=SRect(0,0, Width(*prc), Height(*prc));
	::FillRect(hdcMem, &rcFill, CSmartHandle<HBRUSH>(CreateSolidBrush(rgb)));

	BLENDFUNCTION blend;
	blend.AlphaFormat=0;
	blend.BlendFlags=0;
	blend.SourceConstantAlpha=bAlpha;
	blend.BlendOp=AC_SRC_OVER;
	AlphaBlend(hDC, prc->left, prc->top, Width(*prc), Height(*prc), 
				hdcMem, 0, 0, Width(*prc), Height(*prc), blend);

	}
	DeleteDC(hdcMem);
}


/*
void GradFill(HDC hDC, RECT* prc, COLORREF rgb, BYTE bAlpha)
{
	// NB: This fails on NT4
	TRIVERTEX        vert[2] ;
	GRADIENT_RECT    gRect;
	vert[0].x      = prc->left;
	vert[0].y      = prc->top;
	vert[0].Red    = MulDiv(GetRValue(rgb), 0xff00, 255);
	vert[0].Green  = MulDiv(GetGValue(rgb), 0xff00, 255);
	vert[0].Blue   = MulDiv(GetBValue(rgb), 0xff00, 255);
	vert[0].Alpha  = MulDiv(bAlpha, 0xff00, 255);

	vert[1].x      = prc->right;
	vert[1].y      = prc->bottom;
	vert[1].Red    = vert[0].Red;
	vert[1].Green  = vert[0].Green;
	vert[1].Blue   = vert[0].Blue;
	vert[1].Alpha  = vert[0].Alpha;

	gRect.UpperLeft  = 0;
	gRect.LowerRight = 1;
	GradientFill(hDC, vert,2, &gRect, 1, GRADIENT_FILL_RECT_H);
}
*/

}	// namespace Simple

