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
// FadeWindow.h - declaration of FadeWindow

#ifndef __FADEWINDOW_H
#define __FADEWINDOW_H

namespace Simple
{

#define FWF_HIDE			0x00000001		// Fade out and hide
#define FWF_SHOW			0x00000002		// Show and Fade in
#define FWF_DESTROY			0x00000004		// Fade out and destroy
#define FWF_SYNC			0x00000008		// Don't return until finished
#define FWF_CANCEL			0x00000020		// Cancel any pending operations on specified window
#define FWF_FORCE			0x00000040		// Use even if UseFadeEffects returns false

bool FadeWindow(HWND hWnd, DWORD dwFlags, int iSteps, int iPeriod);

bool UseFadeEffects();
void SetEnableFadeEffects(bool bEnable);
bool GetEnableFadeEffects();

}	// namespace Simple

#endif	// __FADEWINDOW_H

