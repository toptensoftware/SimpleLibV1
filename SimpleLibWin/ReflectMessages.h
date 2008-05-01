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
// ReflectMessages.h - declaration of ReflectMessages

#ifndef __REFLECTMESSAGES_H
#define __REFLECTMESSAGES_H


namespace Simple
{

LRESULT SIMPLEAPI ReflectMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

#define REFLECT_MESSAGES() \
	{ \
		bHandled = TRUE; \
		lResult = ReflectMessages(m_hWnd, uMsg, wParam, lParam, bHandled); \
		if(bHandled) \
			return TRUE; \
	}


}	// namespace Simple

#endif	// __REFLECTMESSAGES_H

