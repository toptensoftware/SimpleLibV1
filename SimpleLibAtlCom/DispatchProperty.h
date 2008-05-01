//////////////////////////////////////////////////////////////////////
//
// SimpleLib ATL COM Version 1.0
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
// DispatchProperty.h - declaration of DispatchProperty

#ifndef __DISPATCHPROPERTY_H
#define __DISPATCHPROPERTY_H

namespace Simple
{

HRESULT SIMPLEAPI SlxGetIDsOfName(IDispatch* pDisp, LPCOLESTR psz, DISPID* pVal);

class CDISPID
{
public:
	CDISPID(DISPID dispid) :
		m_dispid(dispid)
	{
	};
	CDISPID(const int dispid) :
		m_dispid(dispid)
	{
	};
	CDISPID(LPCOLESTR psz) : 
		m_dispid(DISPID_UNKNOWN),
		m_bstr(psz)
	{
	}

	HRESULT Resolve(IDispatch* pDispatch, DISPID* pVal) const
	{
		if (m_bstr)
			return SlxGetIDsOfName(pDispatch, m_bstr, pVal);
		else
			{
			*pVal=m_dispid;
			return S_OK;
			}
	}

	DISPID		m_dispid;
	CComBSTR	m_bstr;
};

HRESULT SIMPLEAPI SlxGetProperty(IDispatch* pDisp, const CDISPID& DispID, VARIANT* pVar);
HRESULT SIMPLEAPI SlxPutProperty(IDispatch* pDisp, const CDISPID& DispID, VARIANT newVal);
HRESULT SIMPLEAPI SlxInitProperty(IDispatch* pDisp, const CDISPID& DispID, VARIANT newVal);


}	// namespace Simple

#endif	// __DISPATCHPROPERTY_H

