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

/////////////////////////////////////////////////////////////////////////////
// ClipDC.h - Handle wrappers

#ifndef _CLIPDC_H
#define _CLIPDC_H


namespace Simple
{

class CClipDC
{
public:
	CClipDC()
	{
		m_hDC=NULL;
		m_hrgnOld=NULL;
	}
	CClipDC(HDC hDC, const RECT& rc)
	{
		m_hDC=NULL;
		m_hrgnOld=NULL;
		Clip(hDC, rc);
	}
	~CClipDC()
	{
		Restore();
	}

	int Clip(HDC hDC, const RECT& rcI)
	{
		Restore();

		// Convert rectangle to device units...
		RECT rc=rcI;
		LPtoDP(hDC, (LPPOINT)&rc.left, 2);

		// Create new clipping region
		HRGN hrgnNew=CreateRectRgnIndirect(&rc);

		// Get the old clipping region
		m_hrgnOld=CreateRectRgn(0,0,0,0);
		if (!GetClipRgn(hDC, m_hrgnOld))
			{
			DeleteObject(m_hrgnOld);
			m_hrgnOld=NULL;
			}

		// Select the new clipping region
		int iRetv=ExtSelectClipRgn(hDC, hrgnNew, RGN_AND);
	
		// Delete the temp region
		DeleteObject(hrgnNew);

		m_hDC=hDC;

		return iRetv;
	}

	void Clip(HDC hDC, HRGN hRgn)
	{
		Restore();

		// Convert origin...
		POINT pt;
		pt.x=0;
		pt.y=0;
		LPtoDP(hDC, &pt, 1);

		// Get the old clipping region
		m_hrgnOld=CreateRectRgn(0,0,0,0);
		if (!GetClipRgn(hDC, m_hrgnOld))
			{
			DeleteObject(m_hrgnOld);
			m_hrgnOld=NULL;
			}

		// Offset the region
		OffsetRgn(hRgn, pt.x, pt.y);

		// Select the new clipping region
		ExtSelectClipRgn(hDC, hRgn, RGN_AND);

		// Restore the region
		OffsetRgn(hRgn, -pt.x, -pt.y);

		m_hDC=hDC;
	}

	void Restore()
	{
		if (m_hDC)
			{
			SelectClipRgn(m_hDC, m_hrgnOld);
			}
		if (m_hrgnOld)
			{
			DeleteObject(m_hrgnOld);
			}
		m_hDC=NULL;
		m_hrgnOld=NULL;
	}

	HDC m_hDC;
	HRGN m_hrgnOld;
};


}	// namespace Simple

#endif		// _CLIPDC_H
