//////////////////////////////////////////////////////////////////////
//
// SimpleLib ATL COM Version 1.0
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
// OleKeyMouse.h - declaration of OleKeyMouse

#ifndef __OLEKEYMOUSE_H
#define __OLEKEYMOUSE_H

namespace Simple
{

#define SHIFT_MASK      0x01
#define CTRL_MASK       0x02
#define ALT_MASK        0x04

#define LEFT_BUTTON     0x01
#define RIGHT_BUTTON    0x02
#define MIDDLE_BUTTON   0x04

short GetOLEKeyboardState();
short GetOLEMouseState();

}	// namespace Simple

#endif	// __OLEKEYMOUSE_H

