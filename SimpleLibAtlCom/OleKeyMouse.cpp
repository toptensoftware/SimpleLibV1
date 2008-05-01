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
// OleKeyMouse.cpp - implementation of OleKeyMouse

#include "stdafx.h"
#include "SimpleLibAtlComBuild.h"

#include "OleKeyMouse.h"


namespace Simple
{

short SIMPLEAPI GetOLEKeyboardState()
{
	short sKeyboard = 0;
	if (GetKeyState(VK_SHIFT) < 0) sKeyboard |= SHIFT_MASK;
	if (GetKeyState(VK_CONTROL) < 0) sKeyboard |= CTRL_MASK;
	if (GetKeyState(VK_MENU) < 0) sKeyboard |= ALT_MASK;
	return sKeyboard;
}

short SIMPLEAPI GetOLEMouseState()
{
	short sMouse = 0;
	if (GetKeyState(VK_LBUTTON) < 0) sMouse |= LEFT_BUTTON;
	if (GetKeyState(VK_RBUTTON) < 0) sMouse |= RIGHT_BUTTON;
	if (GetKeyState(VK_MBUTTON) < 0) sMouse |= MIDDLE_BUTTON;
	return sMouse;
}




}	// namespace Simple
