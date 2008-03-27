//////////////////////////////////////////////////////////////////////////
// MenuIcons.h - declaration of CMenuIcons class

#ifndef __MENUICONS_H
#define __MENUICONS_H

namespace Simple
{

#ifdef __ATLWIN_H__

bool Draw3DCheckmark(HDC hDC, RECT* prc, COLORREF crBg);


// CMenuIcons Class
class CMenuIcons
{
public:
// Construction
			CMenuIcons();
	virtual ~CMenuIcons();

// Operations
	BOOL LoadToolbar(HINSTANCE hInstance, UINT nIDToolbar4, COLORREF rgbMask=RGB(192,192,192), UINT nIDToolbar32=0);
	
	void ClearBold();
	void BoldSubMenu(HMENU hSubMenu);
	void EnableNavKeys(bool bEnable=true);
	void EnableAlphaNav(HMENU hMenu, bool bEnable);
	void Enable(bool bEnable);
	void SubclassWindow(HWND hWnd);
	void UnsubclassWindow();

	/*
BEGIN_MSG_MAP(CMenuIcons)
	MESSAGE_HANDLER(WM_DRAWITEM, OnDrawItem)
	MESSAGE_HANDLER(WM_MEASUREITEM, OnMeasureItem)
	MESSAGE_HANDLER(WM_INITMENUPOPUP, OnInitMenuPopup)
	MESSAGE_HANDLER(WM_MENUSELECT, OnMenuSelect)
	MESSAGE_HANDLER(WM_MENUCHAR, OnMenuChar)
	MESSAGE_HANDLER(WM_SETTINGCHANGE, OnSettingChange)
END_MSG_MAP()
*/

	LRESULT OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMeasureItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnInitMenuPopup(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMenuSelect(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMenuChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSettingChange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnFocusMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	// Handler Insert Pos(CMenuIcons)

// Attributes
	CSmartHandle<HFONT>		m_hFont;

// Operations
	HFONT GetMenuFont();
	void ConvertMenu(HMENU hMenu, bool bConvert);

// Implementation
protected:
// Types
	class CIconInfo
	{
	public:
		CIconInfo(HIMAGELIST hImageList, HIMAGELIST hImageListGray, HIMAGELIST hImageListShadow, int iIcon) : 
			m_hImageList(hImageList),
			m_hImageListGray(hImageListGray),
			m_hImageListShadow(hImageListShadow),
			m_iIcon(iIcon)
		{
		};
		virtual ~CIconInfo()
		{
		};
		HIMAGELIST	m_hImageList;
		HIMAGELIST	m_hImageListGray;
		HIMAGELIST	m_hImageListShadow;
		int			m_iIcon;
	};

	class CItemData
	{
	public:
		static const DWORD Signature;

		CItemData()
		{
			m_dwSignature=Signature;
			m_pIcon=NULL;
		}

		DWORD		m_dwSignature;
		CComBSTR	m_bstrText;
		UINT		m_fType;
		CIconInfo*	m_pIcon;
		bool		m_bBold;
	};

// Attributes
	CVector<HMENU>			m_ConvertedMenus;
	CVector<HIMAGELIST>	m_ImageLists;
	CMap<UINT, CIconInfo*, SValue, SOwnedPtr>	
								m_Icons;
	CMap<HMENU, BOOL>			m_BoldSubMenus;
	CMap<HMENU, BOOL>			m_AlphaNavMenus;
	bool						m_bEnableNavKeys;
	HMENU		m_hSelMenu;		// Selected menu handle
	int			m_iSelMenu;		// Selected menu index
	bool		m_bEnable;
	int			m_iItemHeight;

//	CContainedWindow	m_Focus;

	HHOOK		m_hNavHook;
	WPARAM		m_wParamHooked;
	LPARAM		m_lParamHooked;
	static CMenuIcons*	m_pHookedInstance;

	
	static LRESULT CALLBACK NavHookKeyboardProc(
	  int code,       // hook code
	  WPARAM wParam,  // virtual-key code
	  LPARAM lParam   // keystroke-message information
	);

	HWND		m_hWnd;
	WNDPROC		m_pfnOldWndProc;
	LRESULT WndProc(UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT DefWindowProc(UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndProcStub(HWND HWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Operations
	void InstallNavHook();
	void RemoveNavHook();
	LRESULT OnKeyboardHook(int code, WPARAM wParam, LPARAM lParam);
};

class CDisableMenuIcons
{
public:
	CDisableMenuIcons(CMenuIcons* pMenuIcons) : 
		m_pMenuIcons(pMenuIcons)
	{
		if (m_pMenuIcons)
			m_pMenuIcons->Enable(false);
	}

	~CDisableMenuIcons()
	{
		if (m_pMenuIcons)
			m_pMenuIcons->Enable(true);
	}

	CMenuIcons*	m_pMenuIcons;
};


#endif  // __ATLWIN_H__

}	// namespace Simple

#endif	// __MENUICONS_H

