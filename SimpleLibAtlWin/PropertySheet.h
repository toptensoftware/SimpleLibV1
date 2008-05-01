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
// PropertySheet.h - declaration of PropertySheet

#ifndef __PROPERTYSHEET_H
#define __PROPERTYSHEET_H

#ifdef __ATLWIN_H__
#ifdef __ATLCOM_H__
#ifdef __ATLSNAP_H__

#ifndef _SIMPLELIB_NO_PROPERTYSHEET

#include <prsht.h>

namespace Simple
{

/* PROPERTY PAGE IMPLEMENTATION SHOULD LOOK LIKE THIS


// CMyPropertyPage Class
class CMyPropertyPage : public CPropertyPage<CMyPropertyPage, false>
{
public:
// Construction
			CMyPropertyPage(ivoFilters FilterType);
	virtual ~CMyPropertyPage();

// Attributes
enum { IDD = IDD_MYPROPERTYPAGE };
BEGIN_MSG_MAP(CMyPropertyPage)
END_MSG_MAP()
// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
};


TO DISPLAY THE DIALOG:

	CPropertySheet dlg(_T("My Property Pages"));

	CMyPropertyPage MyPropertyPage;
	dlg.AddPage(MyPropertyPage);

	dlg.DoModal();

*/

class CPropertySheet;

class CPropertyPageBase
{
public:
	CPropertyPageBase()
	{
		m_pSheet=NULL;
	}

	CPropertySheet* GetPropertySheet()
	{
		ASSERT(m_pSheet!=NULL);
		return m_pSheet;
	}

	virtual PROPSHEETPAGE* GetPropSheetPage()=0;

// Implementation
protected:
// Attributes
	CPropertySheet*		m_pSheet;

// Operations
	friend class CPropertySheet;
};

// CPropertyPage Class
template <class T, bool bAutoDelete>
class CPropertyPage : 
	public CPropertyPageBase,
	public CSnapInPropertyPageImpl<T, bAutoDelete>
{
protected:
	typedef CPropertyPage<T, bAutoDelete> _CPropertyPage;

public:
// Construction
			CPropertyPage() {};
	virtual ~CPropertyPage() {};

// Attributes
	bool IsWizard()
	{
		HWND hWndTabs=(HWND)::SendMessage(GetParent(), PSM_GETTABCONTROL, 0, 0);
		return !(::IsWindow(hWndTabs) && (::GetWindowLong(hWndTabs, GWL_STYLE) & WS_DISABLED)==0);
	};

	void SetWizardButtons(DWORD dwButtons)
	{
		if (IsWizard())
			{
			::SendMessage(GetParent(), PSM_SETWIZBUTTONS, 0, dwButtons);
			}
	}

// Operations

// Implementation
protected:
	virtual PROPSHEETPAGE* GetPropSheetPage()
	{
		return &m_psp;
	}
};

// CPropertySheet Class
class CPropertySheet : public CWindowImpl<CPropertySheet>
{
public:
// Construction
			CPropertySheet(LPCTSTR pszCaption);
	virtual ~CPropertySheet();

// Attributes

BEGIN_MSG_MAP(CPropertySheet)
	MESSAGE_HANDLER(WM_SHOWWINDOW, OnShowWindow);
END_MSG_MAP()

// Operations
	void AddPage(PROPSHEETPAGE* pPage);
	INT_PTR DoModal(HWND hWndParent=NULL);
	int FindPage(PROPSHEETPAGE* pPage);

	void AddPage(CPropertyPageBase& Page);
	int GetPageCount() { return m_Pages.GetSize(); };

// Implementation
public:
	PROPSHEETHEADER m_Header;

protected:
// Attributes
	CVector<PROPSHEETPAGE*>	m_Pages;
	bool			m_bInitialShow;


// Operations
	LRESULT OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};

}	// namespace Simple


#endif	//	_SIMPLELIB_NO_PROPERTYSHEET
#endif  // __ATLSNAP_H__
#endif	// __ATLCOM_H__
#endif	// __ATLWIN_H__

#endif	// __PROPERTYSHEET_H

