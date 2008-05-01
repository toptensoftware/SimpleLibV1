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
// SelectObject.h - Handle wrappers

#ifndef _SELECTOBJECT_H
#define _SELECTOBJECT_H


namespace Simple
{

class CSelectObject
{
public:
	CSelectObject()
	{
		m_hDC=NULL;
		m_hOldObject=NULL;
	}
	CSelectObject(HDC hDC, HGDIOBJ h)
	{
		m_hDC=NULL;
		m_hOldObject=NULL;
		Select(hDC, h);
	}

	~CSelectObject()
	{
		Restore();
	}

	void Select(HDC hDC, HGDIOBJ h)
	{
		Restore();
		m_hDC=hDC;
		m_hOldObject=SelectObject(hDC, h);
	}

	void Restore()
	{
		if (m_hDC)
			{
			SelectObject(m_hDC, m_hOldObject);
			m_hDC=NULL;
			m_hOldObject=NULL;
			}
	}

	HDC m_hDC;
	HGDIOBJ m_hOldObject;
};


}	// namespace Simple

#endif		// _SELECTOBJECT_H
