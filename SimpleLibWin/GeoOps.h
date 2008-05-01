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
// GeoOps.h - Geometry operators

#ifndef _GEOOPS_H
#define _GEOOPS_H

namespace Simple
{

#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#endif
#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))
#endif

inline POINT GET_PT_LPARAM(LPARAM lp)
{
	POINT pt={((int)(short)LOWORD(lp)), ((int)(short)HIWORD(lp))};
	return pt;
}



/////////////////////////////////////////////////////////////////////////////
// Size operators

// Constructor
inline SIZE SSize(int cx, int cy)
{
	SIZE size={cx, cy};
	return size;
}

inline SIZE SSize(const SIZEL& sizel)
{
	return (SIZE)sizel;
}

// Conversion Constructor
inline SIZE SSize(const POINT& pt)
{
	return SSize(pt.x, pt.y);
}

// Add
inline SIZE operator + (const SIZE& a, const SIZE& b)
{
	return SSize(a.cx + b.cx, a.cy +b.cy);
}

// Inplace add
inline SIZE& operator += (SIZE& a, const SIZE& b)
{
	a.cx+=b.cx;
	a.cy+=b.cy;
	return a;
}


// Negative
inline SIZE operator - (const SIZE& a)
{
	return SSize(-a.cx, -a.cy);
}

// Subtract
inline SIZE operator - (const SIZE& a, const SIZE& b)
{
	return SSize(a.cx - b.cx, a.cy - b.cy);
}

// Inplace subtract
inline SIZE& operator -= (SIZE& a, const SIZE& b)
{
	a.cx-=b.cx;
	a.cy-=b.cy;
	return a;
}

// Equality
inline bool operator == (const SIZE& a, const SIZE& b)
{
	return a.cx==b.cx && a.cy==b.cy;
}

// Inquality
inline bool operator != (const SIZE& a, const SIZE& b)
{
	return !(a==b);
}



/////////////////////////////////////////////////////////////////////////////
// Point operators

// Constructor
inline POINT SPoint(int x, int y)
{
	POINT pt={x, y};
	return pt;
}

// Conversion constructor
inline POINT SPoint(const POINTL& ptl)
{
	return SPoint(ptl.x, ptl.y);
}

// Conversion Constructor
inline POINT SPoint(const SIZE& size)
{
	return SPoint(size.cx, size.cy);
}

// Add
inline POINT operator + (const POINT& a, const POINT& b)
{
	return SPoint(a.x + b.x, a.y +b.y);
}

// Inplace Add
inline POINT& operator += (POINT& a, const POINT& b)
{
	a.x+=b.x;
	a.y+=b.y;
	return a;
}

// Negative
inline POINT operator - (const POINT& a)
{
	POINT pt={-a.x, -a.y };
	return pt;
}

// Subtract
inline POINT operator - (const POINT& a, const POINT& b)
{
	return SPoint(a.x - b.x, a.y - b.y);
}

// Inplace Subtract
inline POINT& operator -= (POINT& a, const POINT& b)
{
	a.x-=b.x;
	a.y-=b.y;
	return a;
}


// Equality
inline bool operator == (const POINT& a, const POINT& b)
{
	return a.x==b.x && a.y==b.y;
}

// Inquality
inline bool operator != (const POINT& a, const POINT& b)
{
	return !(a==b);
}



/////////////////////////////////////////////////////////////////////////////
// Rectangle operators


// Constructor
inline RECT SRect(int l, int t, int r, int b)
{
	RECT rc;
	rc.left=l;
	rc.right=r;
	rc.top=t;
	rc.bottom=b;
	return rc;
}

// Conversion constructor
inline RECT SRect(const RECTL& rcl)
{
	return SRect(rcl.left, rcl.top, rcl.right, rcl.bottom);
}

// Constructor
inline RECT SRect(const POINT& TopLeft, const POINT& BottomRight)
{
	return SRect(TopLeft.x, TopLeft.y, BottomRight.x, BottomRight.y);
}

// Constructor
inline RECT SRect(const POINT& TopLeft, const SIZE& Size)
{
	return SRect(TopLeft.x, TopLeft.y, TopLeft.x + Size.cx, TopLeft.y + Size.cy);
}


// Offset
inline RECT operator + (const RECT& r, const POINT& p)
{
	return SRect(
				r.left + p.x, 
				r.top + p.y, 
				r.right + p.x,
				r.bottom + p.y );
}

// Inplace Offset
inline RECT& operator += (RECT& r, const POINT& p)
{
	OffsetRect(&r, p.x, p.y);
	return r;
}

// Negative offset
inline RECT operator - (const RECT& r, const POINT& p)
{
	return SRect(
				r.left - p.x, 
				r.top - p.y, 
				r.right - p.x,
				r.bottom - p.y );
}

// Inplace negative offset
inline RECT& operator -= (RECT& r, const POINT& p)
{
	OffsetRect(&r, -p.x, -p.y);
	return r;
}

// Width
inline int Width(const RECT& rc)
{
	return rc.right-rc.left;
}

// Height
inline int Height(const RECT& rc)
{
	return rc.bottom-rc.top;
}

// Set Width
inline void SetWidth(RECT& rc, int iWidth)
{
	rc.right = rc.left + iWidth;
}

// Set Height
inline void SetHeight(RECT& rc, int iHeight)
{
	rc.bottom = rc.top + iHeight;
}

// Set Size
inline void SetSize(RECT& rc, const SIZE& size)
{
	SetWidth(rc, size.cx);
	SetHeight(rc, size.cy);
}

// Size
inline SIZE Size(const RECT& rc)
{
	return SSize(Width(rc), Height(rc));
}

// Top Left
inline POINT TopLeft(const RECT& rc)
{
	return SPoint(rc.left, rc.top);
}
inline POINT& TopLeft(RECT& rc)
{
	return *(POINT*)&rc.left;
}

// Offset a rectangle to a specific position
inline void Move(RECT& rc, POINT pt)
{
	OffsetRect(&rc, pt.x - rc.left, pt.y - rc.top);
}

// Bottom Right
inline POINT BottomRight(const RECT& rc)
{
	return SPoint(rc.right, rc.bottom);
}
inline POINT& BottomRight(RECT& rc)
{
	return *(POINT*)&rc.right;
}

// Horizontal center
inline int HCenter(const RECT& rc)
{
	return (rc.left + rc.right)/2;
}

// Vertical center
inline int VCenter(const RECT& rc)
{
	return (rc.top + rc.bottom)/2;
}

// Center
inline POINT Center(const RECT& rc)
{
	return SPoint(HCenter(rc), VCenter(rc));
}

// Equality
inline bool operator == (const RECT& a, const RECT& b)
{
	return a.left==b.left && a.top==b.top && a.right==b.right && a.bottom==b.bottom;
}

// Inquality
inline bool operator != (const RECT& a, const RECT& b)
{
	return !(a==b);
}



}	// namespace Simple

#endif		// _GEOOPS_H
