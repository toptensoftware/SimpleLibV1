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
// BufferedPaintDC.cpp - implementation of CBufferedPaintDC class

#include "stdafx.h"
#include "SimpleLibWinBuild.h"

#include "BufferedPaintDC.h"

namespace Simple
{

//////////////////////////////////////////////////////////////////////////
// CBufferedPaintDC

// Constructor
CBufferedPaintDC::CBufferedPaintDC(HWND hWnd, PAINTSTRUCT* pPaintStruct, bool bBuffered)
{
	m_hWnd=hWnd;
	m_pPaintStruct=pPaintStruct;
	m_bBuffered=bBuffered;
	if (!m_bBuffered)
		{
		m_hDC=BeginPaint(hWnd, pPaintStruct);
		m_hDCScreen=NULL;
		m_hBitmap=NULL;
		m_hOldBitmap=NULL;
		}
	else
		{
		m_hDCScreen=BeginPaint(hWnd, pPaintStruct);
		m_hDC=CreateCompatibleDC(m_hDCScreen);
		m_hBitmap=CreateCompatibleBitmap(
					m_hDCScreen,
					m_pPaintStruct->rcPaint.right+1, 
					m_pPaintStruct->rcPaint.bottom+1);
		m_hOldBitmap=(HBITMAP)SelectObject(m_hDC, m_hBitmap);
		}
}

// Destructor
CBufferedPaintDC::~CBufferedPaintDC()
{	
	if (m_bBuffered)
		{
		SetMapMode(m_hDC, MM_TEXT);
		BitBlt(m_hDCScreen, 
					m_pPaintStruct->rcPaint.left,
					m_pPaintStruct->rcPaint.top,
					m_pPaintStruct->rcPaint.right-m_pPaintStruct->rcPaint.left,
					m_pPaintStruct->rcPaint.bottom-m_pPaintStruct->rcPaint.top,
					m_hDC, m_pPaintStruct->rcPaint.left, m_pPaintStruct->rcPaint.top, SRCCOPY);
		SelectObject(m_hDC, m_hOldBitmap);
		DeleteDC(m_hDC);
		DeleteObject(m_hBitmap);
		}
	EndPaint(m_hWnd, m_pPaintStruct);
}



}	// namespace Simple
