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
// SimpleToolTip.h - declaration of CSimpleToolTip class

#ifndef __SIMPLETOOLTIP_H
#define __SIMPLETOOLTIP_H

#ifdef __ATLWIN_H__

#ifndef CS_DROPSHADOW
#define CS_DROPSHADOW       0x00020000
#endif

namespace Simple
{

class CSimpleToolTip : public CWindowImpl<CSimpleToolTip>
{
public:
	CSimpleToolTip();
	virtual ~CSimpleToolTip();

	bool Create(HWND hWndParent);

	void Show(const wchar_t* pszText, POINT pt, RECT* prcBounds=NULL);
	void Hide();
	void Update(const wchar_t* pszText, POINT pt, bool bShow);

	static CWndClassInfo& GetWndClassInfo()
	{
		OSVERSIONINFO osvi;
		osvi.dwOSVersionInfoSize=sizeof(osvi);
		GetVersionEx(&osvi);
		bool bXP=osvi.dwPlatformId==VER_PLATFORM_WIN32_NT && osvi.dwMajorVersion>=5 && osvi.dwMinorVersion>=1;

		static CWndClassInfo wc =
		{
			{ sizeof(WNDCLASSEX), (bXP ? CS_DROPSHADOW : 0), StartWindowProc,
			  0, 0, NULL, NULL, NULL, (HBRUSH)(0), NULL, _T("SimpleToolTip"), NULL },
			NULL, NULL, IDC_ARROW, TRUE, 0, _T("") 
		};
		return wc;
	}

BEGIN_MSG_MAP(CSimpleToolTip)
	MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkGnd)
	MESSAGE_HANDLER(WM_SETFONT, OnSetFont)
	MESSAGE_HANDLER(WM_NCHITTEST, OnNCHitTest)
	MESSAGE_HANDLER(WM_TIMER, OnTimer)
END_MSG_MAP()

	LRESULT OnEraseBkGnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSetFont(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNCHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	// Handler Insert Pos(CSimpleToolTip)
	
	CComBSTR	m_bstrText;
	HFONT		m_hFont;
	HWND		m_hWndParent;
	bool		m_bTimerSet;
	RECT		m_rcBounds;
	
	bool IsParentActive();
};

}	// namespace Simple

#endif	// __ATLWIN_H__

#endif	// __SIMPLETOOLTIP_H

