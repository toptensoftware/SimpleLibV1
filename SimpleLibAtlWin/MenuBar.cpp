//////////////////////////////////////////////////////////////////////
//
// SimpleLib ATL Win Version 1.0
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
// MenuBar.cpp - implementation of CMenuBar class

#include "stdafx.h"
#include "SimpleLibAtlWinBuild.h"

#include "MenuBar.h"
#include "MenuIcons.h"

namespace Simple
{

//////////////////////////////////////////////////////////////////////////
// CMenuBar

HHOOK		CMenuBar::m_hMsgHook=NULL;
CMenuBar*	CMenuBar::m_pTrackingInstance=NULL;

// Constructor
CMenuBar::CMenuBar() : 
	m_wndParent(this, 1)
{
	m_hFont=NULL;
	m_hMenuFont=NULL;
	m_hMenuBar1=NULL;
	m_bReleaseMenu=false;
	m_iTrackingItem=-1;
	m_hWndParent=NULL;
	m_iSelectedItem=-1;
	m_bTrackingMouseLeave=false;
	m_bTrackingSubMenu=false;
	m_bOnSubMenu=false;
	m_bCollapsedTrack=true;
	m_bGotMenuPress=false;
	m_hWndMdiClient=NULL;
	m_iMdiButton=-1;
}

// Destructor
CMenuBar::~CMenuBar()
{
}

#define CX_LEFTMARGIN		7
#define CX_ITEMSPACING		12

void CMenuBar::RecalcLayout()
{
	// Calculate placement of all items
	CSmartHandle<HDC> hDC(CreateIC(_T("display"), NULL, NULL, NULL));

	// Remove all items
	m_ItemPlacement.RemoveAll();

	// Select font
	CSelectObject hOldFont(hDC, m_hFont ? m_hFont : GetMenuFont());

	// Get font size
	TEXTMETRIC tm;
	GetTextMetrics(hDC, &tm);

	RECT rc;
	GetClientRect(&rc);
	if (Width(rc)==0 || Height(rc)==0)
		return;

	// Center vertically
	rc.top+=(Height(rc)-tm.tmHeight)/2;
	rc.bottom=rc.top+tm.tmHeight;

	// Start with left indent
	rc.left+=CX_LEFTMARGIN;

	int iCount=m_hMenuBar1 ? GetMenuItemCount() : 0;
	for (int i=0; i<iCount; i++)
	{
		if (i==0 && m_hWndMdiClient)
		{
			int iWidth=GetSystemMetrics(SM_CXSMICON);
			int iHeight=GetSystemMetrics(SM_CYSMICON);

			RECT rcIcon;
			GetClientRect(&rcIcon);
			rcIcon.left=rc.left;
			rcIcon.right=rcIcon.left+iWidth;
			rcIcon.top=VCenter(rcIcon)-iHeight/2;
			rcIcon.bottom=rcIcon.top+iHeight;
			InflateRect(&rcIcon, 2, 2);

			m_ItemPlacement.Add(new RECT(rcIcon));

			rc.left=rcIcon.right+3;
		}
		else
		{
			// Get menu text
			TCHAR sz[MAX_PATH];
			GetMenuString(i, sz, MAX_PATH, MF_BYPOSITION);
									
			RECT rcItem=rc;
			rcItem.right=32767;
			DrawText(hDC, sz, -1, &rcItem, DT_CALCRECT);
			
			rc.right=rc.left + Width(rcItem);

			RECT* prc=new RECT(rc);
			InflateRect(prc, 5, 3);
		
			m_ItemPlacement.Add(prc);
	
			rc.left=rc.right+CX_ITEMSPACING;
		}

	}
}

int CMenuBar::HitTest(POINT pt)
{
	for (int i=0; i<m_ItemPlacement.GetSize(); i++)
	{
		if (PtInRect(m_ItemPlacement[i], pt))
			return i;
	}
	return -1;
}


//////////////////////////////////////////////////////////////////////////
// Message handlers

// WM_CREATE handler
LRESULT CMenuBar::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// Store parent window handle...
	m_hWndParent=GetParent();
	while (::GetWindowLong(m_hWndParent, GWL_STYLE) & WS_CHILD)
	{
		m_hWndParent=::GetParent(m_hWndParent);
	}


	// TODO: Add initialisation code
	return 0;
}

// WM_DESTROY handler
LRESULT CMenuBar::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_bReleaseMenu && m_hMenuBar1)
	{
		DestroyMenu(m_hMenuBar1);
		m_hMenuBar1=NULL;
	}
	return 0;
}

// WM_PAINT handler
LRESULT CMenuBar::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// Begin Paint
	PAINTSTRUCT ps;
	CBufferedPaintDC hDC(m_hWnd, &ps);

	RECT rc;
	GetClientRect(&rc);
	FillRect(hDC, &rc, GetSysColorBrush(COLOR_BTNFACE));

	{
		CSelectObject hOldFont(hDC, m_hFont ? m_hFont : GetMenuFont());

		SetBkMode(hDC, TRANSPARENT);

		COLORREF colorWindow = GetSysColor(COLOR_WINDOW);
		COLORREF colorMenuBar = GetSysColor(COLOR_3DFACE);
		COLORREF colorMenu = GetMixedColor(colorWindow, colorMenuBar);
		COLORREF colorBitmap = GetMixedColor(colorMenuBar, colorWindow);
		COLORREF colorSel = GetXPHighlightColor(XPSTATE_HOT, colorMenu);
		COLORREF colorBorder = GetSysColor(COLOR_HIGHLIGHT);
		COLORREF colorCheckedBitmap = GetXPHighlightColor(XPSTATE_CHECKED, colorBitmap);

		for (int i=0; i<m_ItemPlacement.GetSize(); i++)
		{
			// Get menu text
			TCHAR sz[MAX_PATH];
			GetMenuString(i, sz, MAX_PATH, MF_BYPOSITION);

			bool bSelected=i==m_iSelectedItem;

			if (bSelected)
			{
				FillRect(hDC, m_ItemPlacement[i], CSmartHandle<HBRUSH>(CreateSolidBrush(colorSel)));
				FrameRect(hDC, m_ItemPlacement[i], CSmartHandle<HBRUSH>(CreateSolidBrush(colorBorder)));
				SetTextColor(hDC, GetSysColor(COLOR_MENUTEXT));
			}
			else
			{
				SetTextColor(hDC, GetSysColor(COLOR_MENUTEXT));
			}

			if (m_hWndMdiClient && i==0)
			{
				HWND hWndChild=(HWND)::SendMessage(m_hWndMdiClient, WM_MDIGETACTIVE, 0, 0);
				if (hWndChild)
				{
					HICON hIcon=(HICON)SendMessage(hWndChild, WM_GETICON, ICON_SMALL, 0);
					DrawIconEx(hDC, m_ItemPlacement[i]->left+2, m_ItemPlacement[i]->top+2, hIcon,
									GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 
									0, NULL, DI_NORMAL);
				}
				
			}
			else
			{
				DrawText(hDC, sz, -1, m_ItemPlacement[i], DT_SINGLELINE|DT_CENTER|DT_VCENTER);
			}
		}

	}

	if (m_hWndMdiClient)
	{
		UINT uStates[3]={DFCS_CAPTIONMIN, DFCS_CAPTIONRESTORE, DFCS_CAPTIONCLOSE };
		for (int i=0; i<3; i++)
		{
			RECT rc;
			GetMdiButtonRect(i, &rc);

			DrawFrameControl(hDC, &rc, DFC_CAPTION, uStates[i] | (m_iMdiButton==i ? DFCS_PUSHED : 0));
		}
	}

	return 0;
}

void CMenuBar::GetMdiButtonRect(int iButton, RECT* prc)
{
	GetClientRect(prc);

	SIZE size=SSize(16,16);

	prc->top=VCenter(*prc)-size.cy/2;
	prc->bottom=prc->top+size.cy;
	prc->right-=prc->top;
	prc->right-=(2-iButton)*size.cx;
	prc->left=prc->right-size.cx;

	if (iButton<2)
		OffsetRect(prc, -3, 0);
}

// WM_SETFONT handler
LRESULT CMenuBar::OnSetFont(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_hFont=(HFONT)wParam;
	m_hMenuFont.Release();
	if (lParam)
		{
		// TODO: recalculate layout if necessary
		Invalidate();
		}

	return 0;
}

// WM_GETFONT handler
LRESULT CMenuBar::OnGetFont(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return (LRESULT)m_hFont;
}

// WM_GETDLGCODE handler
LRESULT CMenuBar::OnGetDlgCode(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// TODO: return appropriate dlg code
	return 0;
}



// MBM_SETMENU Handler
LRESULT CMenuBar::OnSetMenu(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_bReleaseMenu && m_hMenuBar1)
	{
		DestroyMenu(m_hMenuBar1);
		m_hMenuBar1=NULL;
	}
	
	// Crack parameters
	m_hMenuBar1=(HMENU)wParam;
	m_bReleaseMenu=!!lParam;

	RecalcLayout();
	Invalidate();

	return (BOOL)TRUE;
}


// MBM_GETMENU Handler
LRESULT CMenuBar::OnGetMenu(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return (LRESULT)(HMENU)m_hMenuBar1;
}

// WM_SIZE Handler
LRESULT CMenuBar::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	RecalcLayout();
	return 0;
}

// MBM_REDRAW Handler
LRESULT CMenuBar::OnRedraw(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	RecalcLayout();
	Invalidate();
	return 0;
}

int CMenuBar::HitTestMdiButton(POINT pt)
{
	if (!m_hWndMdiClient)
		return -1;

	for (int i=0; i<3; i++)
	{
		RECT rc;
		GetMdiButtonRect(i, &rc);
		if (PtInRect(&rc, pt))
			return i;
	}

	return -1;

}

void CMenuBar::InvalidateMdiButton(int iButton)
{
	if (iButton>=0)
	{
		RECT rc;
		GetMdiButtonRect(iButton, &rc);
		InvalidateRect(&rc);
	}
}


// WM_LBUTTONDOWN Handler
LRESULT CMenuBar::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT pt=GET_PT_LPARAM(lParam);

	int iMdiButton=HitTestMdiButton(pt);
	if (iMdiButton>=0)
	{	
		// Track MDI button
		m_iMdiButton=iMdiButton;
		InvalidateMdiButton(m_iMdiButton);
		SetCapture();
		return 0;
	}

	//::PostMessage(hWndChild, 0x0313, 0, MAKELPARAM(100, 100));


	// Which menu item?
	int iItem=HitTest(pt);
	if (iItem<0)
		return 0;

	// Store original click position
	DWORD dwTickClick=GetTickCount();

	m_bCloseByClick0=false;
	TrackMenu(iItem, false);

	if (m_bCloseByClick0 && GetTickCount()-dwTickClick<GetDoubleClickTime())
	{
		HWND hWndChild=(HWND)::SendMessage(m_hWndMdiClient, WM_MDIGETACTIVE, 0, 0);
		if (::IsWindow(hWndChild))
		{
			::PostMessage(hWndChild, WM_SYSCOMMAND, SC_CLOSE, 0);
		}
	}

	return 0;
}

// WM_LBUTTONDBLCLK Handler
LRESULT CMenuBar::OnLButtonDblClk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return OnLButtonDown(uMsg, wParam, lParam, bHandled);
}



#define WM_SYSKEYFIRST  WM_SYSKEYDOWN
#define WM_SYSKEYLAST   WM_SYSDEADCHAR
#define WM_NCMOUSEFIRST WM_NCMOUSEMOVE
#define WM_NCMOUSELAST  WM_NCMBUTTONDBLCLK

static bool SIMPLEAPI IsInputMessage(UINT msg)
{
	if ((msg>=WM_MOUSEFIRST) && (msg<=WM_MOUSELAST)) return TRUE;
	if ((msg>=WM_NCMOUSEFIRST) && (msg<=WM_NCMOUSELAST)) return TRUE;
	if ((msg>=WM_KEYFIRST) && (msg<=WM_KEYLAST)) return TRUE;
	if ((msg>=WM_SYSKEYFIRST) && (msg<=WM_SYSKEYLAST)) return TRUE;

	return FALSE;
}


LRESULT CMenuBar::TrackingHookProc(int code, WPARAM wp, LPARAM lp)
{
	return (code==MSGF_MENU && m_pTrackingInstance && m_pTrackingInstance->TrackProc(((MSG*)lp))) ? TRUE
		: CallNextHookEx(m_hMsgHook, code, wp, lp);
}

void CMenuBar::TrackMenu(int iIndex, bool bCollapsed)
{
	// Cancel tracking?
	if (iIndex<0)
	{
		if (m_iTrackingItem>=0)
		{
			::PostMessage(m_hWndParent, WM_CANCELMODE, 0, 0);
		}
		return;
	}

	// Redundant?
	if (m_iTrackingItem==iIndex && m_bNextTrackCollapsed==bCollapsed)
	{
		return;
	}

	// Store next item
	m_iNextTrackItem=iIndex;
	m_bNextTrackCollapsed=bCollapsed;

	// If already tracking, unwind stack...
	if (m_iTrackingItem>=0)
	{
		::PostMessage(m_hWndParent, WM_CANCELMODE, 0, 0);
		return;
	}

	// Subclass parent while we display the menu so we can capture WM_MENUSELECT
	m_wndParent.SubclassWindow(m_hWndParent);

	m_ptMouseLast=SPoint(-10000, -10000);

	// Track menus until closed...
	while (m_iNextTrackItem>=0)
	{
		m_bGotMenuPress=false;

		// Reset flags
		m_bTrackingSubMenu=false;
		m_bOnSubMenu=false;

		// Store item being tracked
		m_iTrackingItem=m_iNextTrackItem;

		if (m_bNextTrackCollapsed)
		{
			m_iSelectedItem=m_iNextTrackItem;
			Invalidate();

			// Clear it
			m_iNextTrackItem=-1;


			bool bContinue=true;
			while (bContinue)
			{
				MSG msg;
				while (!PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE))
				{
					WaitMessage();
				}

				if (msg.message==WM_SYSKEYDOWN && (msg.wParam==VK_F4 || msg.wParam==VK_MENU || msg.wParam==VK_F10))
				{
					bContinue=false;
					GetMessage(&msg, NULL, NULL, NULL);
				}
				else
				{
					switch (msg.message)
					{
						case WM_CANCELMODE:
							GetMessage(&msg, NULL, NULL, NULL);
							bContinue=false;
							break;

						case WM_CAPTURECHANGED:
						case WM_LBUTTONDOWN:
						case WM_RBUTTONDOWN:
						case WM_MBUTTONDOWN:
						case WM_NCLBUTTONDOWN:
						case WM_NCRBUTTONDOWN:
						case WM_NCMBUTTONDOWN:
						case WM_ACTIVATE:
						case WM_NCACTIVATE:
							bContinue=false;
							break;

						default:
						{
							GetMessage(&msg, NULL, NULL, NULL);
							if (!TrackProc(&msg) && !IsInputMessage(msg.message))
							{
								DispatchMessage(&msg);
							}
							break;
						}
					}
				}
			}
		}
		else
		{
			// Get the sub menu
			HMENU hSubMenu=GetSubMenu(m_iNextTrackItem);

			// Get item position
			RECT rcItem=*m_ItemPlacement[m_iNextTrackItem];

			if ((GetStyle() & WS_VISIBLE)==0)
			{
				rcItem.bottom=rcItem.top;
			}

			ClientToScreen(&rcItem);

			// Store selected item and force repaint
			m_iSelectedItem=m_iNextTrackItem;
			Invalidate();

			// Clear it
			m_iNextTrackItem=-1;



			// Install hook
			ASSERT(m_hMsgHook==NULL);
			m_hMsgHook = SetWindowsHookEx(WH_MSGFILTER, TrackingHookProc, NULL, ::GetCurrentThreadId());
			m_pTrackingInstance=this;

			// Display menu...
			TPMPARAMS tpm;
			tpm.cbSize=sizeof(tpm);
			tpm.rcExclude=rcItem;
			m_bCollapsedTrack=false;
			UINT nCmd=TrackPopupMenuEx(hSubMenu, TPM_LEFTBUTTON|TPM_VERTICAL|TPM_RETURNCMD, rcItem.left, rcItem.bottom, m_hWndParent, &tpm);
			m_bCollapsedTrack=true;

			if (nCmd)
			{
				if (m_iSelectedItem==0 && m_hWndMdiClient)
				{
					HWND hWndChild=(HWND)::SendMessage(m_hWndMdiClient, WM_MDIGETACTIVE, 0, 0);
					if (::IsWindow(hWndChild))
						::PostMessage(hWndChild, WM_SYSCOMMAND, nCmd, 0);
				}
				else
				{
					::PostMessage(m_hWndParent, WM_COMMAND, nCmd, 0);
				}
			}

			// Remove hook...
			::UnhookWindowsHookEx(m_hMsgHook);
			m_pTrackingInstance=NULL;
			m_hMsgHook=NULL;
		}
	}

	// Reset flags
	m_bTrackingSubMenu=false;
	m_bOnSubMenu=false;

	// Remove subclass
	m_wndParent.UnsubclassWindow();
 
 	// Clear tracking flag, clean up
	m_iTrackingItem=-1;
	m_iSelectedItem=-1;
	Invalidate();
}

void CMenuBar::PostKey(int iKey)
{
	::PostMessage(m_hWndParent, WM_KEYDOWN, iKey, 1);
	::PostMessage(m_hWndParent, WM_KEYUP, iKey, 1);
}


bool CMenuBar::TrackProc(MSG* pMsg)
{
	switch (pMsg->message)
	{
		case WM_MOUSEMOVE:
		{
			POINT pt;
			GetCursorPos(&pt);
			ScreenToClient(&pt);

			// Actually moved
			if (m_ptMouseLast==pt)
				return false;
			m_ptMouseLast=pt;

			int iItem=HitTest(pt);
			if (iItem>=0)
			{
				TrackMenu(iItem, m_bCollapsedTrack);
			}
			break;
		}

		case WM_LBUTTONDOWN:
		{
			if (!m_bCollapsedTrack)
			{
				POINT pt;
				GetCursorPos(&pt);
				ScreenToClient(&pt);

				int iItem=HitTest(pt);
				if (iItem==m_iTrackingItem)
				{
					if (iItem==0)
					{
						m_bCloseByClick0=true;
					}

					::PostMessage(m_hWndParent, WM_CANCELMODE, 0, 0);
					return true;
				}
			}
		}


		case WM_KEYDOWN:
		{
			int iNewTrackItem=m_iTrackingItem;

			// Cancel collapsed track
			if (pMsg->wParam==VK_ESCAPE)
			{
				if (m_bCollapsedTrack)
				{
					::PostMessage(m_hWndParent, WM_CANCELMODE, 0, 0);
				}
				else
				{
					TrackMenu(m_iTrackingItem, true);
				}

				return true;
			}

			// Up/down on collapsed track
			if ((pMsg->wParam==VK_DOWN || pMsg->wParam==VK_UP) && m_bCollapsedTrack)
			{
				TrackMenu(m_iTrackingItem, false);
				PostKey(VK_DOWN);
				return true;
			}

			// Left?
			if (pMsg->wParam==VK_LEFT && !m_bTrackingSubMenu)
				{
				iNewTrackItem--;
				if (iNewTrackItem<0)
					iNewTrackItem=GetMenuItemCount()-1;
				}

			if (pMsg->wParam==VK_RIGHT && !m_bOnSubMenu)
				{
				iNewTrackItem++;
				if (iNewTrackItem>=GetMenuItemCount())
					iNewTrackItem=0;
				}

			if (iNewTrackItem!=m_iTrackingItem)
				{
				TrackMenu(iNewTrackItem, m_bCollapsedTrack);
				if (!m_bCollapsedTrack)
					PostKey(VK_DOWN);
				return false;
				}
			break;
		}
	}

	return false;
}

// TPM_SETPARENT Handler
LRESULT CMenuBar::OnSetParent(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// Crack parameters
	m_hWndParent=(HWND)wParam;
	return (LPARAM)TRUE;
}

// WM_MOUSEMOVE Handler
LRESULT CMenuBar::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (GetCapture()==m_hWnd)
	{
		int iNewButton=HitTestMdiButton(GET_PT_LPARAM(lParam));
		if (iNewButton!=m_iMdiButton)
		{
			InvalidateMdiButton(m_iMdiButton);
			m_iMdiButton=iNewButton;
			InvalidateMdiButton(m_iMdiButton);
		}
		return 0;
	}

	if (!m_bTrackingMouseLeave)
		{
		TRACKMOUSEEVENT e;
		e.cbSize=sizeof(TRACKMOUSEEVENT);
		e.dwFlags=TME_LEAVE;
		e.dwHoverTime=0;
		e.hwndTrack=m_hWnd;
		_TrackMouseEvent(&e);
		m_bTrackingMouseLeave=true;
		}

	int iItem=HitTest(GET_PT_LPARAM(lParam));
	if (iItem!=m_iSelectedItem)
	{
		m_iSelectedItem=iItem;
		Invalidate();
	}

	return 0;
}


// WM_LBUTTONUP Handler
LRESULT CMenuBar::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (GetCapture()==m_hWnd)
	{
		if (m_iMdiButton>=0)
		{
			HWND hWndChild=(HWND)::SendMessage(m_hWndMdiClient, WM_MDIGETACTIVE, 0, 0);
			if (::IsWindow(hWndChild))
			{
				switch (m_iMdiButton)
				{
					case 0:
						::PostMessage(hWndChild, WM_SYSCOMMAND, SC_MINIMIZE, 0);
						break;
					case 1:
						::PostMessage(hWndChild, WM_SYSCOMMAND, SC_RESTORE, 0);
						break;
					case 2:
						::PostMessage(hWndChild, WM_SYSCOMMAND, SC_CLOSE, 0);
						break;
				}
			}


			InvalidateMdiButton(m_iMdiButton);
			m_iMdiButton=-1;
		}

		ReleaseCapture();
	}
	return 0;
}



// WM_MOUSELEAVE Handler
LRESULT CMenuBar::OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_iTrackingItem<0 && m_iSelectedItem>=0)
	{
		m_iSelectedItem=-1;
		Invalidate();
	}

	m_bTrackingMouseLeave=false;
	return 0;
}

// WM_MENUSELECT Handler
LRESULT CMenuBar::OnParentMenuSelect(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// Crack parameters
	HMENU hMenu=(HMENU)lParam;
	int	iIndex=(int)LOWORD(wParam);

	// Store state
	m_bOnSubMenu=::GetSubMenu(hMenu, iIndex)!=NULL;
	m_bTrackingSubMenu=hMenu!=GetSubMenu(m_iTrackingItem);

	// Never handled...
	bHandled=FALSE;
	return 0;
}

LRESULT CMenuBar::OnParentInitMenuPopup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_iSelectedItem==0 && m_hWndMdiClient)
	{
		HWND hWndChild=(HWND)::SendMessage(m_hWndMdiClient, WM_MDIGETACTIVE, 0, 0);
		if (hWndChild)
		{
			SendMessage(hWndChild, uMsg, wParam, lParam);
			EnableMenuItem((HMENU)wParam, SC_MOVE, MF_GRAYED|MF_BYCOMMAND);
			EnableMenuItem((HMENU)wParam, SC_SIZE, MF_GRAYED|MF_BYCOMMAND);
			EnableMenuItem((HMENU)wParam, SC_RESTORE, MF_ENABLED|MF_BYCOMMAND);
			EnableMenuItem((HMENU)wParam, SC_MINIMIZE, MF_ENABLED|MF_BYCOMMAND);
			EnableMenuItem((HMENU)wParam, SC_MAXIMIZE, MF_GRAYED|MF_BYCOMMAND);
			bHandled=TRUE;
			return 0;
		}		
	}
	bHandled=FALSE;
	return 0;
}

int CMenuBar::ItemFromMnemonic(TCHAR ch)
{
	if (ch==0)
		return -1;

	// Scan top level menu for matching character
	int iCount=GetMenuItemCount();
	for (int i=0; i<iCount; i++)
	{
		TCHAR sz[MAX_PATH];
		GetMenuString(i, sz, MAX_PATH, MF_BYPOSITION);

		LPTSTR pszAmper=_tcschr(sz, _T('&'));
		if (pszAmper && toupper(pszAmper[1])==toupper(ch))
			return i;
	}

	return -1;
}


// MBM_PRETRANSLATEMESSAGE Handler
LRESULT CMenuBar::OnPreTranslateMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// Crack parameters
	MSG* pMessage=(MSG*)lParam;

	if (pMessage->message!=WM_SYSCHAR && 
		pMessage->message!=WM_SYSKEYDOWN && 
		pMessage->message!=WM_SYSKEYUP)
		return FALSE;

	if (!(pMessage->hwnd==m_hWndParent || ::IsChild(m_hWndParent, pMessage->hwnd)))
		return FALSE;

	// System key to open menu?
	if (pMessage->message==WM_SYSCHAR && m_iTrackingItem<0)
	{
		int iItem=ItemFromMnemonic((TCHAR)pMessage->wParam);
		if (iItem>=0)
		{
			PostKey(VK_DOWN);
			TrackMenu(iItem, false);
			return TRUE;
		}
	}

//	if (pMessage->message==WM_KEYDOWN || pMessage->message==WM_KEYUP)
//	{
//		m_bGotMenuPress=false;
//	}

	if (pMessage->message==WM_SYSKEYDOWN)
	{
		m_bGotMenuPress=(pMessage->wParam==VK_MENU || pMessage->wParam==VK_F10);
		return FALSE;
	}

	if (pMessage->message==WM_SYSKEYUP && (pMessage->wParam==VK_MENU || pMessage->wParam==VK_F10))
	{
		if (m_bGotMenuPress)
		{
			TrackMenu(0, true);
		}

		return TRUE;
	}


	return (BOOL)FALSE;
}

// WM_ACTIVATE Handler
LRESULT CMenuBar::OnParentActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (LOWORD(wParam)==WA_INACTIVE)
	{
		// Need two for some reason?
		::PostMessage(m_hWndParent, WM_CANCELMODE, 0, 0);
		::PostMessage(m_hWndParent, WM_CANCELMODE, 0, 0);
	}
	return 0;
}

// MBM_GETHEIGHT Handler
LRESULT CMenuBar::OnGetHeight(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// Calculate placement of all items
	CSmartHandle<HDC> hDC(CreateIC(_T("display"), NULL, NULL, NULL));

	// Select font
	CSelectObject hOldFont(hDC, m_hFont ? m_hFont : GetMenuFont());

	// Get font size
	TEXTMETRIC tm;
	GetTextMetrics(hDC, &tm);

	return tm.tmHeight*7/4;
}


// MBM_SETMDIMODE Handler
LRESULT CMenuBar::OnSetMdiMode(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_hWndMdiClient=(HWND)wParam;
	RecalcLayout();
	Invalidate();
	return TRUE;
}

// WM_ERASEBKGND Handler
LRESULT CMenuBar::OnEraseBkGnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return TRUE;
}


HFONT CMenuBar::GetMenuFont()
{
	if (!m_hFont) 
		{
		NONCLIENTMETRICS info;
		info.cbSize = sizeof(info);
		SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(info), &info, 0);
		m_hMenuFont=CreateFontIndirect(&info.lfMenuFont);
		}

	return m_hMenuFont;
}

int CMenuBar::GetMenuItemCount()
{
	if (m_hWndMdiClient)
		return ::GetMenuItemCount(m_hMenuBar1)+1;			// Extra one for the system menu
	else
		return ::GetMenuItemCount(m_hMenuBar1);
}

int CMenuBar::GetMenuString(UINT nIDItem, LPTSTR lpString, int nMaxCount, UINT uFlag)
{
	if (m_hWndMdiClient)
	{
		if (uFlag & MF_BYPOSITION)
		{
			if (nIDItem==0)
			{
				wcscpy_s(lpString, nMaxCount, _T("&-"));
				return 2;
			}
			else
			{
				return ::GetMenuString(m_hMenuBar1, nIDItem-1, lpString, nMaxCount, uFlag);
			}
		}
		else
		{
			return ::GetMenuString(m_hMenuBar1, nIDItem, lpString, nMaxCount, uFlag);
		}
	}
	else
	{
		return ::GetMenuString(m_hMenuBar1, nIDItem, lpString, nMaxCount, uFlag);
	}

}

HMENU CMenuBar::GetSubMenu(int iIndex)
{
	if (m_hWndMdiClient)
	{
		if (iIndex==0)
		{
			HWND hWndChild=(HWND)::SendMessage(m_hWndMdiClient, WM_MDIGETACTIVE, 0, 0);
			if (::IsWindow(hWndChild))
			{
				return ::GetSystemMenu(hWndChild, FALSE);
			}
			else
			{
				return NULL;
			}
		}
		else
		{
			return ::GetSubMenu(m_hMenuBar1, iIndex-1);
		}
	}
	else
	{
		return ::GetSubMenu(m_hMenuBar1, iIndex);
	}
}




void SIMPLEAPI RegisterMenuBar()
{
	CMenuBar::Register();
}

void SIMPLEAPI UnregisterMenuBar()
{
	CMenuBar::Unregister();
}

}	// namespace Simple


