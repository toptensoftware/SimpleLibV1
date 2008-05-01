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
// CommandState.h - declaration of CommandState

#ifndef __COMMANDSTATE_H
#define __COMMANDSTATE_H

namespace Simple
{

// Command flags
#define CSF_DISABLED		0x01
#define CSF_CHECKED			0x02
#define CSF_IGNORE			0x04

// Message sent to window to get its state
#define WM_COMMANDSTATE		WM_AFXLAST		// Unlikely clash for WM_COMMAND state

HMENU SIMPLEAPI CommandStateGetMenu();
int SIMPLEAPI CommandStateGetMenuIndex();

void SIMPLEAPI CommandStateUpdateMenu(HWND hWnd, HMENU hMenu);
void SIMPLEAPI CommandStateUpdateToolBar(HWND hWnd, HWND hWndToolBar, int iFirst=0, int iLast=-1);
void SIMPLEAPI CommandStateSetMenuText(const wchar_t* psz);

// LRESULT OnCommandState(WORD wID, BOOL& bHandled);

#define COMMANDSTATE_HANDLER(id, func) \
	if(uMsg == WM_COMMANDSTATE && id == LOWORD(wParam)) \
	{ \
		bHandled = TRUE; \
		lResult = func(LOWORD(wParam), bHandled); \
		if(bHandled) \
			return TRUE; \
	}

// Use to mark end of command state handlers to save continuing through
// entire message map
#define END_COMMANDSTATE_HANDLERS() \
	if (uMsg==WM_COMMANDSTATE) \
		return FALSE;

#ifdef SLX_SMART_COMMANDSTATE_HANDLERS

// Replacement COMMAND_ID_HANDLER to set probe flag for COMMANDSTATE_DISABLEUNHANDLED
#undef COMMAND_ID_HANDLER
#define COMMAND_ID_HANDLER(id, func) \
	if(uMsg == WM_COMMAND && id == LOWORD(wParam)) \
	{ \
		bHandled = TRUE; \
		lResult = func(HIWORD(wParam), LOWORD(wParam), (HWND)lParam, bHandled); \
		if(bHandled) \
			return TRUE; \
	} \
	if(uMsg==WM_COMMANDSTATE && id==LOWORD(wParam)) \
	{ \
		*((LPARAM*)lParam)|=1; \
	}

// Replacement COMMAND_RANGE_HANDLER to set probe flag for COMMANDSTATE_DISABLEUNHANDLED
#undef COMMAND_RANGE_HANDLER
#define COMMAND_RANGE_HANDLER(idFirst, idLast, func) \
	if(uMsg == WM_COMMAND && LOWORD(wParam) >= idFirst  && LOWORD(wParam) <= idLast) \
	{ \
		bHandled = TRUE; \
		lResult = func(HIWORD(wParam), LOWORD(wParam), (HWND)lParam, bHandled); \
		if(bHandled) \
			return TRUE; \
	} \
	if(uMsg==WM_COMMANDSTATE && LOWORD(wParam) >= idFirst  && LOWORD(wParam) <= idLast) \
	{ \
		*((LPARAM*)lParam)|=1; \
	}

// Disable all commands that don't have a COMMAND_ID_HANDLER
#define COMMANDSTATE_DISABLEUNHANDLED() \
	if (uMsg==WM_COMMANDSTATE) \
	{ \
		lResult=(*((LPARAM*)lParam) & 1) ? 0 : CSF_DISABLED; \
		return TRUE; \
	};

#endif	// SLX_SMART_COMMANDSTATE_HANDLERS


// Route all commands, notifies and command state messages to another window
#define COMMAND_ROUTE(targetwindow) \
	if(uMsg == WM_COMMAND || uMsg==WM_NOTIFY || uMsg==WM_COMMANDSTATE) \
	{ \
		if (targetwindow.ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult, 0)) \
			return TRUE; \
	}

// Route all commands, notifies and command state messages to another window 
//		but on if it or a child has focus
#define COMMAND_FOCUS_ROUTE(targetwindow) \
	if((uMsg == WM_COMMAND || uMsg==WM_NOTIFY || uMsg==WM_COMMANDSTATE) && (targetwindow==GetFocus() || ::IsChild(targetwindow, GetFocus()))) \
	{ \
		if (targetwindow.ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult, 0)) \
			return TRUE; \
	}

#define COMMAND_MDI_ROUTE(mdiclient) \
	if (uMsg==WM_COMMAND || uMsg==WM_NOTIFY || uMsg==WM_COMMANDSTATE) \
	{ \
		HWND hWndActiveChild=(HWND)::SendMessage(mdiclient, WM_MDIGETACTIVE, 0, 0); \
		if (hWndActiveChild) \
		{ \
			lResult=SendMessage(hWndActiveChild, uMsg, wParam, lParam); \
			return TRUE; \
		} \
	}

// Disable all commands
#define COMMANDSTATE_DISABLEALL() \
	if (uMsg==WM_COMMANDSTATE) \
	{ \
		lResult=CSF_DISABLED; \
		return TRUE; \
	};

// Enable all commands
#define COMMANDSTATE_ENABLEALL() \
	if (uMsg==WM_COMMANDSTATE) \
	{ \
		lResult=0; \
		return TRUE; \
	};

// Disable a specific command
#define COMMANDSTATE_DISABLE(id) \
	if(uMsg == WM_COMMANDSTATE && id == LOWORD(wParam)) \
	{ \
		bHandled = TRUE; \
		lResult = CSF_DISABLED; \
		return TRUE; \
	}

// Enable a specific command
#define COMMANDSTATE_ENABLE(id) \
	if(uMsg == WM_COMMANDSTATE && id == LOWORD(wParam)) \
	{ \
		bHandled = TRUE; \
		lResult = 0; \
		return TRUE; \
	}

// Enable a specific command
#define COMMANDSTATE_ENABLE_RANGE(id1, id2) \
	if(uMsg == WM_COMMANDSTATE && LOWORD(wParam)>=(id1) && LOWORD(wParam)<=(id2)) \
	{ \
		bHandled = TRUE; \
		lResult = 0; \
		return TRUE; \
	}

// Enable a specific command
#define COMMANDSTATE_IGNORE_RANGE(id1, id2) \
	if(uMsg == WM_COMMANDSTATE && LOWORD(wParam)>=(id1) && LOWORD(wParam)<=(id2)) \
	{ \
		bHandled = TRUE; \
		lResult = CSF_IGNORE; \
		return TRUE; \
	}

#define START_FOCUS_GROUP(idCtrl) \
	{ \
	HWND hWndFocus=GetFocus(); \
	HWND hWndCtrl=GetDlgItem(idCtrl); \
	if (hWndFocus==hWndCtrl || ::IsChild(hWndCtrl, hWndFocus)) \
		{


#define END_FOCUS_GROUP() \
		} \
	}


}	// namespace Simple

#endif	// __COMMANDSTATE_H


