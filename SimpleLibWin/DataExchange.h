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
// DataExchange.h - declaration of DataExchange

#ifndef __DATAEXCHANGE_H
#define __DATAEXCHANGE_H

namespace Simple
{

CUniString SIMPLEAPI GetWindowText(HWND hWnd);
CUniString SIMPLEAPI GetDlgItemText(HWND hWnd, int iID);

// CADXExchange - maintains info about the current exchange state
class CADXExchange
{
public:
// Constructor
	CADXExchange(HWND hWndDlg, bool bSave, LCID lcid=LOCALE_USER_DEFAULT);

// Operations
	HWND PrepareControl(UINT uID);
	void Error(const wchar_t* pszMessage);
	void SetErrorCaption(const wchar_t* pszCaption);
	
// Attributes
	bool		m_bSave;
	bool		m_bOK;
	LCID		m_lcid;
	HWND		m_hWndCurrent;
	HWND		m_hWndDlg;
	CUniString	m_strErrorCaption;
};


// Exchange functions (call from within DoDataExchange)
void SIMPLEAPI ADX_Text(CADXExchange* pDX, int nIDC, CUniString& str);
void SIMPLEAPI ADX_Text(CADXExchange* pDX, int nIDC, int& iVal);
void SIMPLEAPI ADX_Text(CADXExchange* pDX, unsigned int nIDC, unsigned int& iVal);
void SIMPLEAPI ADX_Text(CADXExchange* pDX, unsigned int nIDC, double& dblVal, int iDP=-1);
void SIMPLEAPI ADX_CheckBox(CADXExchange* pDX, int nIDC, int& iValue);
void SIMPLEAPI ADX_CheckBox(CADXExchange* pDX, int nIDC, VARIANT_BOOL& bValue);
void SIMPLEAPI ADX_CheckBox(CADXExchange* pDX, int nIDC, bool& bValue);
void SIMPLEAPI ADX_AddListItems(HWND hWnd, const wchar_t* pszItems, UINT msg_ADDSTRING);
void SIMPLEAPI ADX_CBIndex(CADXExchange* pDX, int nIDC, int& iIndex, const wchar_t* pszItems=NULL);
void SIMPLEAPI ADX_LBIndex(CADXExchange* pDX, int nIDC, int& iIndex, const wchar_t* pszItems=NULL);
void SIMPLEAPI ADX_CBData(CADXExchange* pDX, int nIDC, LPARAM& dwData);
void SIMPLEAPI ADX_LBData(CADXExchange* pDX, int nIDC, LPARAM& dwData);
void SIMPLEAPI ADX_Radio(CADXExchange* pDX, int nIDC, int& value);
void SIMPLEAPI ADX_CheckBit(CADXExchange* pDX, int nIDC, DWORD& iValue, DWORD iBit);

// Validation functions (call from within DoDataExchange)
void SIMPLEAPI ADX_Required(CADXExchange* pDX, const wchar_t* psz);
void SIMPLEAPI ADX_RangeInt(CADXExchange* pDX, int iValue, int iMin, int iMax);
void SIMPLEAPI ADX_RangeUInt(CADXExchange* pDX, unsigned int iValue, unsigned int iMin, unsigned int iMax);
void SIMPLEAPI ADX_LimitInt(CADXExchange* pDX, int Value, int iLimit, bool bMustBeGreater, bool bInclusive);
void SIMPLEAPI ADX_LimitUInt(CADXExchange* pDX, unsigned int Value, unsigned int iLimit, bool bMustBeGreater, bool bInclusive);
void SIMPLEAPI ADX_RangeDouble(CADXExchange* pDX, double dblValue, double dblMin, double dblMax, int iDP=-1);
void SIMPLEAPI ADX_LimitDouble(CADXExchange* pDX, double Value, double dblLimit, bool bMustBeGreater, bool bInclusive, int iDP=-1);
void SIMPLEAPI ADX_Validate(CADXExchange* pDX, int nIDC, bool bCondition, const wchar_t* pszMessage);


}	// namespace Simple

#endif	// __DATAEXCHANGE_H

