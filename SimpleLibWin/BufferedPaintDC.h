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
// BufferedPaintDC.h - declaration of CBufferedPaintDC class

#ifndef __BUFFEREDPAINTDC_H
#define __BUFFEREDPAINTDC_H

namespace Simple
{

// Helper class for buffered painting
class CBufferedPaintDC
{
public:
	CBufferedPaintDC(HWND hWnd, PAINTSTRUCT* pPaintStruct, bool bBuffered=true);
	~CBufferedPaintDC();
	operator HDC() { return m_hDC; }

	HWND			m_hWnd;
	HDC				m_hDC;
	HDC				m_hDCScreen;
	HBITMAP			m_hBitmap;
	HBITMAP			m_hOldBitmap;
	PAINTSTRUCT*	m_pPaintStruct;
	bool			m_bBuffered;

};

}	// namespace Simple

#endif	// __BUFFEREDPAINTDC_H

