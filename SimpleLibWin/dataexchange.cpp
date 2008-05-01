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


#include "stdafx.h"
#include "SimpleLibWinBuild.h"

#include "DataExchange.h"
#include "MessageBox.h"

namespace Simple
{

CUniString SIMPLEAPI GetWindowText(HWND hWnd)
{
	CUniString str;
	int iLen=GetWindowTextLength(hWnd);
	::GetWindowText(hWnd, str.GetBuffer(iLen), iLen+1);
	return str;
}

CUniString SIMPLEAPI GetDlgItemText(HWND hWnd, int iID)
{
	return GetWindowText(GetDlgItem(hWnd, iID));
}


/////////////////////////////////////////////////////////////////////////////
// CADXExchange

CADXExchange::CADXExchange(HWND hWndDlg, bool bSave, LCID lcid) :
	m_hWndDlg(hWndDlg),
	m_bSave(bSave),
	m_bOK(true),
	m_lcid(lcid)
{
};

HWND CADXExchange::PrepareControl(UINT uID)
{
	m_hWndCurrent=GetDlgItem(m_hWndDlg, uID);
	ASSERT(m_hWndCurrent!=NULL);
	return m_hWndCurrent;
}

void CADXExchange::Error(const wchar_t* pszMessage)
{
	// Quit if already displayed a message
	if (!m_bOK)
		return;

	m_bOK=false;

	// Get caption of dialog
	SlxMessageBox(pszMessage, MB_OK|MB_ICONHAND, m_strErrorCaption);

	// Set focus to the control in error
	SetFocus(m_hWndCurrent);

	// Select text?
	if (SendMessage(m_hWndCurrent, WM_GETDLGCODE, 0, 0) & DLGC_HASSETSEL)
		SendMessage(m_hWndCurrent, EM_SETSEL, 0, -1);
};


void CADXExchange::SetErrorCaption(const wchar_t* pszCaption)
{
	m_strErrorCaption=pszCaption;
}

/////////////////////////////////////////////////////////////////////////////
// ADX_xxx/ADX_xxx exchange and validation functions


// Simple text exchange
void SIMPLEAPI ADX_Text(CADXExchange* pDX, int nIDC, CUniString& str)
{
	// Get the control
	HWND hWnd=pDX->PrepareControl(nIDC);

	if (pDX->m_bSave)
		{
		str=GetWindowText(hWnd);
		}
	else
		{
		SetWindowText(hWnd, str);
		}
};

// Integer text exchange
void SIMPLEAPI ADX_Text(CADXExchange* pDX, int nIDC, int& iVal)
{
	CUniString str;

	if (!pDX->m_bSave)
		{
		BSTR bstr=NULL;
		VarBstrFromI4(iVal, pDX->m_lcid, 0, &bstr);
		str=bstr;
		SysFreeString(bstr);
		}

	ADX_Text(pDX, nIDC, str);

	if (pDX->m_bSave)
		{
		if (str.IsEmpty())
			{
			iVal=0;
			}
		else
			{
			ASSERT(sizeof(long)==sizeof(int));
			if (FAILED(VarI4FromStr(const_cast<OLECHAR*>(str.sz()), pDX->m_lcid, 0, (long*)&iVal)))
				pDX->Error(L"Invalid value");
			}
		}
};

// Unsigned integer text exchange
void SIMPLEAPI ADX_Text(CADXExchange* pDX, unsigned int nIDC, unsigned int& iVal)
{
	CUniString str;

	if (!pDX->m_bSave)
		{
		BSTR bstr=NULL;
		VarBstrFromUI4(iVal, pDX->m_lcid, 0, &bstr);
		str=bstr;
		SysFreeString(bstr);
		}

	ADX_Text(pDX, nIDC, str);

	if (pDX->m_bSave)
		{
		if (str.IsEmpty())
			{
			iVal=0;
			}
		else
			{
			ASSERT(sizeof(unsigned long)==sizeof(unsigned int));
			if (FAILED(VarUI4FromStr(const_cast<OLECHAR*>(str.sz()), pDX->m_lcid, 0, (unsigned long*)&iVal)))
				pDX->Error(L"Invalid value");
			}
		}
};


// Double text exchange
void SIMPLEAPI ADX_Text(CADXExchange* pDX, unsigned int nIDC, double& dblVal)
{
	CUniString str;

	if (!pDX->m_bSave)
		{
		BSTR bstr=NULL;
		VarBstrFromR8(dblVal, pDX->m_lcid, 0, &bstr);
		str=bstr;
		SysFreeString(bstr);
		}

	ADX_Text(pDX, nIDC, str);

	if (pDX->m_bSave)
		{
		if (str.IsEmpty())
			{
			dblVal=0;
			}
		else
			{
			if (FAILED(VarR8FromStr(const_cast<OLECHAR*>(str.sz()), pDX->m_lcid, 0, &dblVal)))
				pDX->Error(L"Invalid value");
			}
		}
};


// Check that a string has been entered
void SIMPLEAPI ADX_Required(CADXExchange* pDX, const wchar_t* psz)
{
	// Quit if not saving or already have an error
	if (!pDX->m_bSave || !pDX->m_bOK)
		return;

	// Quit if have value
	if (!IsEmptyString(psz))
		return;

	pDX->Error(L"Required value missing");
};


// Get the buddy up/down control associated with a control
HWND SIMPLEAPI GetBuddyControl(HWND hWnd)
{
	HWND hWndBuddy=GetWindow(hWnd, GW_HWNDNEXT);
	if (!hWndBuddy)
		return NULL;

	DWORD dwStyle=GetWindowLong(hWndBuddy, GWL_STYLE);

	// Quick check 1
	// Generally spinners don't have tab stop set...
	if (dwStyle & WS_TABSTOP)
		return NULL;

	// Quick check 2, must have these styles
	if ((dwStyle & (UDS_AUTOBUDDY|UDS_SETBUDDYINT)) != (UDS_AUTOBUDDY|UDS_SETBUDDYINT))
		return NULL;

	// Finally, double check with class name
	wchar_t szClass[MAX_PATH];
	GetClassName(hWndBuddy, szClass, MAX_PATH);
	if (lstrcmpi(szClass, L"msctls_updown32")!=0)
		return NULL;

	return hWndBuddy;
}

// Validate the range of an integer number, also sets the range on buddy up/down if found
void SIMPLEAPI ADX_RangeInt(CADXExchange* pDX, int iValue, int iMin, int iMax)
{
	if (!pDX->m_bSave)
		{
		// Try to set range on spinner control that follows
		HWND hWndBuddy=GetBuddyControl(pDX->m_hWndCurrent);

		// Send it...
		if (hWndBuddy)
			SendMessage(hWndBuddy, UDM_SETRANGE32, iMin, iMax);
		return;
		}

	if (iValue<iMin || iValue>iMax)
		{
		pDX->Error(Format(L"Value out of range.  Please enter a number between %i and %i", iMin, iMax));
		}
}

// Validate the range of an unsigned integer number, also sets the range on buddy up/down if found
void SIMPLEAPI ADX_RangeUInt(CADXExchange* pDX, unsigned int iValue, unsigned int iMin, unsigned int iMax)
{
	if (!pDX->m_bSave)
		{
		// Try to set range on spinner control that follows
		HWND hWndBuddy=GetBuddyControl(pDX->m_hWndCurrent);

		// Send it...
		if (hWndBuddy)
			SendMessage(hWndBuddy, UDM_SETRANGE32, iMin, iMax);
		return;
		}

	if (iValue<iMin || iValue>iMax)
		{
		pDX->Error(Format(L"Value out of range.  Please enter a number between %u and %u", iMin, iMax));
		}
}

// Check that an integer number is greater than/less than and/or equal to a value
void SIMPLEAPI ADX_LimitInt(CADXExchange* pDX, int Value, int iLimit, bool bMustBeGreater, bool bInclusive)
{
	if (!pDX->m_bSave)
		return;

	// If inclusive and value matches, must be included regardless of direction
	if (iLimit==Value && bInclusive)
		return;

	if (bMustBeGreater && Value>iLimit)
		return;
	if (!bMustBeGreater && Value<iLimit)
		return;

	CUniString str(L"Value out of range.  Please enter a number that is ");
	str+=bMustBeGreater ? L"greater " : L"less ";
	str+=L"than ";
	if (bInclusive)
		str+=L"or equal to ";

	str+=Format(L"%i.", iLimit);

	pDX->Error(str);
}

// Check that an unsigned integer number is greater than/less than and/or equal to a value
void SIMPLEAPI ADX_LimitUInt(CADXExchange* pDX, unsigned int Value, unsigned int iLimit, bool bMustBeGreater, bool bInclusive)
{
	if (!pDX->m_bSave)
		return;

	// If inclusive and value matches, must be included regardless of direction
	if (iLimit==Value && bInclusive)
		return;

	if (bMustBeGreater && Value>iLimit)
		return;
	if (!bMustBeGreater && Value<iLimit)
		return;

	CUniString str(L"Value out of range.  Please enter a number that is ");
	str+=bMustBeGreater ? L"greater " : L"less ";
	str+=L"than ";
	if (bInclusive)
		str+=L"or equal to ";

	str+=Format(L"%u.", iLimit);

	pDX->Error(str);
}

// Check that a double value is within range
void SIMPLEAPI ADX_RangeDouble(CADXExchange* pDX, double dblValue, double dblMin, double dblMax)
{
	if (!pDX->m_bSave)
		return;

	if (dblValue<dblMin || dblValue>dblMax)
		{
		pDX->Error(Format(L"Value out of range.  Please enter a number between %f and %f", dblMin, dblMax));
		}
}

// Check that a double number is greater than/less than and/or equal to a value
void SIMPLEAPI ADX_LimitDouble(CADXExchange* pDX, double Value, double dblLimit, bool bMustBeGreater, bool bInclusive)
{
	if (!pDX->m_bSave)
		return;

	// If inclusive and value matches, must be included regardless of direction
	if (dblLimit==Value && bInclusive)
		return;

	if (bMustBeGreater && Value>dblLimit)
		return;
	if (!bMustBeGreater && Value<dblLimit)
		return;

	CUniString str(L"Value out of range.  Please enter a number that is ");
	str+=bMustBeGreater ? L"greater " : L"less ";
	str+=L"than ";
	if (bInclusive)
		str+=L"or equal to ";
	str+=Format(L"%g.", dblLimit);

	pDX->Error(str);
}

// Simple check box exchange
void SIMPLEAPI ADX_CheckBox(CADXExchange* pDX, int nIDC, int& iValue)
{
	// Get the control
	HWND hWnd=pDX->PrepareControl(nIDC);

	if (!pDX->m_bSave)	
		{
		SendMessage(hWnd, BM_SETCHECK, iValue, 0);

		DWORD dwStyle=GetWindowLong(hWnd, GWL_STYLE);
		if (((dwStyle & 0x0000000f) == BS_AUTO3STATE) && iValue!=2)
			{
			SendMessage(hWnd, BM_SETSTYLE, (dwStyle & 0xFFFFFFF0) | BS_AUTOCHECKBOX, 1);
			}
		}
	else
		{
		iValue=int(SendMessage(hWnd, BM_GETCHECK, 0, 0));
		}
}

void SIMPLEAPI ADX_CheckBox(CADXExchange* pDX, int nIDC, VARIANT_BOOL& bValue)
{
	// Get the control
	HWND hWnd=pDX->PrepareControl(nIDC);

	if (!pDX->m_bSave)	
		{
		SendMessage(hWnd, BM_SETCHECK, bValue ? 1 : 0, 0);
		}
	else
		{
		bValue=BOOL2VBOOL(SendMessage(hWnd, BM_GETCHECK, 0, 0));
		}
}

void SIMPLEAPI ADX_CheckBox(CADXExchange* pDX, int nIDC, bool& bValue)
{
	VARIANT_BOOL b=BOOL2VBOOL(bValue);
	ADX_CheckBox(pDX, nIDC, b);
	if (pDX->m_bSave)
		{
		bValue=!!b;
		}
}



// Helper function to add a \n separated list of strings
// to either a list box or combo box.  msg_ADDSTRING should be either
// LB_ADDSTRING or CB_ADDSTRING
void SIMPLEAPI ADX_AddListItems(HWND hWnd, const wchar_t* pszItems, UINT msg_ADDSTRING)
{
	// Quit if not specified...
	if (!pszItems)
		return;

	const wchar_t* p=pszItems;
	const wchar_t* pszItem=p;
	while (true)
		{
		// End of item?
		if (p[0]==L'\0' || p[0]==L'\n')
			{
			int iLen=int(p-pszItem);
			if (iLen)	
				{
				CUniString str(pszItem, iLen);

				// Add to list
				SendMessage(hWnd, msg_ADDSTRING, 0, (LPARAM)static_cast<const wchar_t*>(str));
				}

			// Quit if end of string
			if (p[0]==L'\0')
				return;

			// Save start of next item
			pszItem=p+1;
			}

		p++;
		}
}

// Exchange list box index, optionally adding items to the list first...
void SIMPLEAPI ADX_CBIndex(CADXExchange* pDX, int nIDC, int& iIndex, const wchar_t* pszItems)
{
	HWND hWnd=pDX->PrepareControl(nIDC);

	if (!pDX->m_bSave)
		{
		// Don't do it twice...
		if (SendMessage(hWnd, CB_GETCOUNT, 0, 0)==0)
			{
			// Add items...
			ADX_AddListItems(hWnd, pszItems, CB_ADDSTRING);
			}

		// Select it
		SendMessage(hWnd, CB_SETCURSEL, iIndex, 0);
		}
	else
		{
		// Get selected item...
		iIndex=int(SendMessage(hWnd, CB_GETCURSEL, 0, 0));
		}
};


void SIMPLEAPI ADX_CBData(CADXExchange* pDX, int nIDC, LPARAM& lData)
{
	HWND hWnd=pDX->PrepareControl(nIDC);
	if (!pDX->m_bSave)
		{
		int iCount=(int)SendMessage(hWnd, CB_GETCOUNT, 0, 0);
		for (int i=0; i<iCount; i++)
			{
			LPARAM lDataThis=SendMessage(hWnd, CB_GETITEMDATA, i, 0);
			if (lDataThis==lData)
				{
				SendMessage(hWnd, CB_SETCURSEL, i, 0);
				return;
				}
			}

		// Select nothing
		SendMessage(hWnd, CB_SETCURSEL, -1, 0);
		}
	else
		{
		// Get selected item...
		int iIndex=(int)SendMessage(hWnd, CB_GETCURSEL, 0, 0);
		if (iIndex<0)
#ifdef _WIN64
			lData=0xFFFFFFFFFFFFFFFF;
#else
			lData=0xFFFFFFFF;
#endif
		else
			lData=SendMessage(hWnd, CB_GETITEMDATA, iIndex, 0);
		}
}

void SIMPLEAPI ADX_LBData(CADXExchange* pDX, int nIDC, LPARAM& lData)
{
	HWND hWnd=pDX->PrepareControl(nIDC);
	if (!pDX->m_bSave)
		{
		int iCount=(int)SendMessage(hWnd, LB_GETCOUNT, 0, 0);
		for (int i=0; i<iCount; i++)
			{
			LPARAM lDataThis=SendMessage(hWnd, LB_GETITEMDATA, i, 0);
			if (lDataThis==lData)
				{
				SendMessage(hWnd, LB_SETCURSEL, i, 0);
				return;
				}
			}

		// Select nothing
		SendMessage(hWnd, LB_SETCURSEL, -1, 0);
		}
	else
		{
		// Get selected item...
		int iIndex=(int)SendMessage(hWnd, LB_GETCURSEL, 0, 0);
		if (iIndex<0)
#ifdef _WIN64
			lData=0xFFFFFFFFFFFFFFFF;
#else
			lData=0xFFFFFFFF;
#endif
		else
			lData=SendMessage(hWnd, LB_GETITEMDATA, iIndex, 0);
		}
}

// Exchange list box index, optionally adding items to the list first...
void SIMPLEAPI ADX_LBIndex(CADXExchange* pDX, int nIDC, int& iIndex, const wchar_t* pszItems)
{
	HWND hWnd=pDX->PrepareControl(nIDC);

	if (!pDX->m_bSave)
		{
		// Don't do it twice...
		if (SendMessage(hWnd, LB_GETCOUNT, 0, 0)==0)
			{
			// Add items...
			ADX_AddListItems(hWnd, pszItems, LB_ADDSTRING);
			}

		// Select it
		SendMessage(hWnd, LB_SETCURSEL, iIndex, 0);
		}
	else
		{
		// Get selected item...
		iIndex=int(SendMessage(hWnd, LB_GETCURSEL, 0, 0));
		}
};

// Exchange selected radio button
void SIMPLEAPI ADX_Radio(CADXExchange* pDX, int nIDC, int& value)
{
	HWND hWnd=pDX->PrepareControl(nIDC);
	ASSERT(GetWindowLong(hWnd, GWL_STYLE) & WS_GROUP);			// Must start with a group	
	ASSERT(SendMessage(hWnd, WM_GETDLGCODE, 0, 0) & DLGC_RADIOBUTTON);	// Must be a radio button

	// Setup default value if not found
	if (pDX->m_bSave)
		value = -1;

	int iButton=0;
	while (true)
		{
		// Is it a radio button?
		if (SendMessage(hWnd, WM_GETDLGCODE, 0, 0) & DLGC_RADIOBUTTON)
			{
			if (pDX->m_bSave)
				{
				// Is it checked?
				if (SendMessage(hWnd, BM_GETCHECK, 0, 0))
					{
					// Return it...
					value=iButton;
					return;
					}
				}
			else
				{
				// Select/deselect it...
				SendMessage(hWnd, BM_SETCHECK, (iButton==value), 0);
				}

			// Update index counter
			iButton++;
			}

		// Get next button
		hWnd=GetWindow(hWnd, GW_HWNDNEXT);

		// Quit if no more windows or start or next group
		if (!hWnd || (GetWindowLong(hWnd, GWL_STYLE) & WS_GROUP))
			break;
		}
}


void SIMPLEAPI ADX_CheckBit(CADXExchange* pDX, int nIDC, DWORD& iValue, DWORD iBit)
{
	bool bValue=(iValue & iBit)!=0;
	ADX_CheckBox(pDX, nIDC, bValue);
	if (pDX->m_bSave)
		{
		iValue=(iValue & ~iBit) | (bValue ? iBit : 0);
		}
}


// Generic validation function.  If bCondition isn't true, pszMessage is displayed
// and focus set back to control nIDC.
void SIMPLEAPI ADX_Validate(CADXExchange* pDX, int nIDC, bool bCondition, const wchar_t* pszMessage)
{
	if (!pDX->m_bSave || bCondition)
		return;

	pDX->PrepareControl(nIDC);
	pDX->Error(pszMessage);
};



}	// namespace Simple
