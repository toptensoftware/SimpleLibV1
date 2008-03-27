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
// VariantParams.cpp - implementation of VariantParams

#include "stdafx.h"
#include "SimpleLibAtlComBuild.h"
#include "VariantParams.h"

namespace Simple
{


// Check if a variant parameter is specified
BOOL IsParameterSpecified(VARIANT& var)
{
	return !((V_VT(&var)==VT_EMPTY) || 
			 ((V_VT(&var)==VT_ERROR) && (V_ERROR(&var)==DISP_E_PARAMNOTFOUND)));
}

// Return S_OK if integer, S_FALSE if string, or error code if failed
HRESULT VariantToStringOrInt(VARIANT& Item, int& i, CComBSTR& bstr)
{
	HRESULT hr;
	// Convert to long first
	CComVariant vaTemp;

	hr=vaTemp.ChangeType(VT_I4, &Item);
	if (SUCCEEDED(hr))
		{
		i=V_I4(&vaTemp);
		return S_OK;
		}

	if (V_VT(&Item)==VT_BSTR)
		{
		bstr=V_BSTR(&Item);
		return S_FALSE;
		}

	// Convert to string
	hr=vaTemp.ChangeType(VT_BSTR, &Item);
	if (FAILED(hr))
		return hr;

	// Return string
	bstr=V_BSTR(&vaTemp);
	return S_FALSE;
}

HRESULT VariantToStringOrIntOrObject(VARIANT& Item, int* pi, BSTR* pbstrVal, IUnknown** pVal)
{
	CComVariant varTemp;
	if (SUCCEEDED(varTemp.ChangeType(VT_UNKNOWN, &Item)))
		{
		*pVal=V_UNKNOWN(&varTemp);
		V_VT(&varTemp)=VT_EMPTY;
		return S_OK;
		}

	if (SUCCEEDED(varTemp.ChangeType(VT_I4, &Item)) && V_VT(&Item)!=VT_BSTR)
		{
		*pi=V_I4(&varTemp);
		return S_OK;
		}

	if (V_VT(&Item)==VT_BSTR)
		{
		*pbstrVal=SysAllocString(V_BSTR(&Item));
		return S_OK;
		}

	if (SUCCEEDED(varTemp.ChangeType(VT_BSTR, &Item)))
		{
		*pbstrVal=V_BSTR(&varTemp);
		V_VT(&varTemp)=VT_EMPTY;
		return S_OK;
		}

	return DISP_E_TYPEMISMATCH;
}


HRESULT GetOptionalString(CComBSTR& bstr, VARIANT& var, LPCOLESTR pszDefault)
{
	if (IsParameterSpecified(var))
		{
		if (V_VT(&var)==VT_BSTR)
			{
			bstr=V_BSTR(&var);
			return S_OK;
			}

		CComVariant varTemp;
		RETURNIFFAILED(varTemp.ChangeType(VT_BSTR, &var));
		bstr=V_BSTR(&varTemp);
		return S_OK;
		}
	else
		{
		bstr=pszDefault;
		return S_FALSE;
		}
}

HRESULT GetOptionalInt(int& i, VARIANT& var, int iDefault)
{
	if (IsParameterSpecified(var))
		{
		CComVariant varTemp;
		RETURNIFFAILED(varTemp.ChangeType(VT_I4, &var));
		i=V_I4(&varTemp);
		return S_OK;
		}
	else
		{
		i=iDefault;
		return S_FALSE;
		}
}

HRESULT GetOptionalUInt(unsigned int& i, VARIANT& var, unsigned int iDefault)
{
	if (IsParameterSpecified(var))
		{
		CComVariant varTemp;
		RETURNIFFAILED(varTemp.ChangeType(VT_UI4, &var));
		i=V_I4(&varTemp);
		return S_OK;
		}
	else
		{
		i=iDefault;
		return S_FALSE;
		}
}

HRESULT GetOptionalShort(short& i, VARIANT& var, short iDefault)
{
	if (IsParameterSpecified(var))
		{
		CComVariant varTemp;
		RETURNIFFAILED(varTemp.ChangeType(VT_I2, &var));
		i=V_I2(&varTemp);
		return S_OK;
		}
	else
		{
		i=iDefault;
		return S_FALSE;
		}
}

HRESULT GetOptionalUShort(unsigned short& i, VARIANT& var, unsigned short iDefault)
{
	if (IsParameterSpecified(var))
		{
		CComVariant varTemp;
		RETURNIFFAILED(varTemp.ChangeType(VT_UI2, &var));
		i=V_I2(&varTemp);
		return S_OK;
		}
	else
		{
		i=iDefault;
		return S_FALSE;
		}
}

HRESULT GetOptionalObject(VARIANT& var, REFIID riid, void** ppvObject)
{
	if (!IsParameterSpecified(var))
		{
		*ppvObject=NULL;
		return S_FALSE;
		}

	CComVariant varTemp;
	RETURNIFFAILED(varTemp.ChangeType(VT_UNKNOWN, &var));

	if (!V_UNKNOWN(&varTemp))
		{
		ppvObject=NULL;
		return S_FALSE;
		}

	return V_UNKNOWN(&varTemp)->QueryInterface(riid, ppvObject);
}

HRESULT GetOptionalDouble(double& dbl, VARIANT& var, double dblDefault)
{
	if (IsParameterSpecified(var))
		{
		CComVariant varTemp;
		RETURNIFFAILED(varTemp.ChangeType(VT_R8, &var));
		dbl=V_R8(&varTemp);
		return S_OK;
		}
	else
		{
		dbl=dblDefault;
		return S_FALSE;
		}
}




}	// namespace Simple
