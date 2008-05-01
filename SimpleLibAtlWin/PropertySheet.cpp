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

#include "stdafx.h"
#include "SimpleLibAtlWinBuild.h"

#include "PropertySheet.h"

namespace Simple
{

// Constructor
CPropertySheet::CPropertySheet(LPCTSTR pszCaption)
{
	// Init PROPSHEETHEADER
	memset(&m_Header, 0, sizeof(PROPSHEETHEADER));
	m_Header.dwSize=sizeof(PROPSHEETHEADER);
	m_Header.hInstance=_AtlBaseModule.GetResourceInstance();
	m_Header.pszCaption=pszCaption;
}

// Destructor
CPropertySheet::~CPropertySheet()
{
}

// Add a page
void CPropertySheet::AddPage(PROPSHEETPAGE* pPage)
{
	m_Pages.Add(pPage);
}

void CPropertySheet::AddPage(CPropertyPageBase& Page)
{
	ASSERT(Page.m_pSheet==NULL);
	Page.m_pSheet=this;
	m_Pages.Add(Page.GetPropSheetPage());
}

int CPropertySheet::FindPage(PROPSHEETPAGE* pPage)
{
	return m_Pages.Find(pPage);
}



static CPropertySheet* g_pPropSheetCreating;

int CALLBACK PropSheetProc(HWND hWndDlg, UINT nMsg, LPARAM lParam)
{
	if (nMsg==PSCB_INITIALIZED)
	{
		g_pPropSheetCreating->SubclassWindow(hWndDlg);
	}

	return 0;
}


// Display property sheet modally...
INT_PTR CPropertySheet::DoModal(HWND hWndParent)
{
	_ASSERTE(m_Pages.GetSize()>0);

	m_bInitialShow=true;

	if (!hWndParent)
		hWndParent=GetActiveWindow();

	// Setup rest of header...
	m_Header.hwndParent=hWndParent;

	// Allocate memory for all prop sheet 
	m_Header.ppsp=(PROPSHEETPAGE*)_alloca(sizeof(PROPSHEETPAGE) * m_Pages.GetSize());
	m_Header.nPages=m_Pages.GetSize();

	// Copy PROPSHEETPAGE structures from all CPropertyPageImpl's
	for (int i=0; i<m_Pages.GetSize(); i++)
		{
		// Prepare the page
		const_cast<PROPSHEETPAGE*>(m_Header.ppsp)[i]=*m_Pages[i];
		}


	m_Header.dwFlags |= PSH_PROPSHEETPAGE;

	if ((m_Header.dwFlags & PSH_USECALLBACK)==0)
	{
		m_Header.pfnCallback=PropSheetProc;
		m_Header.dwFlags |= PSH_USECALLBACK;
		g_pPropSheetCreating=this;
	}

	// Display the property sheet
	return PropertySheet(&m_Header);
}

// WM_SHOWWINDOW handler
LRESULT CPropertySheet::OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_bInitialShow && wParam)
	{
		CenterWindow(GetParent());
		m_bInitialShow=false;
	}
	bHandled=FALSE;
	return 0;
}


}

