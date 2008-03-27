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

#ifdef __ATLWIN_H__

namespace Simple
{

void ADX_Text(CADXExchange* pDX, int nIDC, CComBSTR& str);

// Template function to do ADX exchange with any window class that
// derives from CWindow and has a function:
//			 void DoDataExchange(CADXExchange* pDX)
template <class T>
bool ADXUpdateData(T* pDlg, bool bSave)
{
	// Construct an exchange object
	CADXExchange dx(*static_cast<CWindow*>(pDlg), bSave);

	// Do the exchange...
	pDlg->DoDataExchange(&dx);

	return dx.m_bOK;
}


// CAdxDialogImpl - replacement for CDialogImpl that has handlers to
//						do the data exchange automatically.
// NB: Your derived class must still have IDOK, IDCANCEL and OnInitDialog 
//		in its message map - but not necessarily handlers for them.
template <class T, class TBase = CWindow>
class CAdxDialogImpl : public CDialogImpl<T, TBase>
{
public:
	typedef CAdxDialogImpl<T, TBase>	_CAdxDialogImpl;

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CenterWindow();
		ADXUpdateData(this, false);
		return 0;
	}
	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		if (ADXUpdateData(this, true))
			EndDialog(wID);
		return 0;
	}
	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		EndDialog(wID);
		return 0;
	}
	virtual void DoDataExchange(CADXExchange* pDX)=0;
};


// Typical dialog would look like this:

/*

/////////////////////////////////////////////////////////////////////////////
// CMyDialog

class CMyDialog : 
	public CAdxDialogImpl<CMyDialog>
{
public:
			CMyDialog();
	virtual ~CMyDialog();

	enum { IDD = IDD_MYDIALOG };

	virtual void DoDataExchange(CADXExchange* pDX);

BEGIN_MSG_MAP(CMyDialog)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	COMMAND_ID_HANDLER(IDOK, OnOK)
	COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
END_MSG_MAP()
// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};



/////////////////////////////////////////////////////////////////////////////
// CMyDialog

// Constructor
CMyDialog::CMyDialog()
{
}

// Destructor
CMyDialog::~CMyDialog()
{
}

void CMyDialog::DoDataExchange(CADXExchange* pDX)
{
	// Add calls to ADX_xxx / ADV_xxx functons
}


/////////////////////////////////////////////////////////////////////////////
// Message handlers

// WM_INITDIALOG handler
LRESULT CMyDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	_CAdxDialogImpl::OnInitDialog(uMsg, wParam, lParam, bHandled);

	// Additional initialization here...

	return 1;  // Let the system set the focus
}


*/

}	// namespace Simple

#endif	// __ATLWIN_H__

