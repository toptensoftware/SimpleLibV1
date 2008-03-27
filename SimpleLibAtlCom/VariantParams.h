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
// VariantParams.h - declaration of VariantParams

#ifndef __VARIANTPARAMS_H
#define __VARIANTPARAMS_H

namespace Simple
{

// Check if a variant parameter was "missing"
BOOL IsParameterSpecified(VARIANT& var);

// Convert a variant to either an integer or a string
HRESULT VariantToStringOrInt(VARIANT& Item, int& i, CComBSTR& bstr);
HRESULT VariantToStringOrIntOrObject(VARIANT& Item, int* pi, BSTR* pbstrVal, IUnknown** pVal);

HRESULT GetOptionalString(CComBSTR& bstr, VARIANT& var, LPCOLESTR pszDefault);
HRESULT GetOptionalShort(short& i, VARIANT& var, short iDefault);
HRESULT GetOptionalUShort(unsigned short& i, VARIANT& var, unsigned short iDefault);
HRESULT GetOptionalInt(int& i, VARIANT& var, int iDefault);
HRESULT GetOptionalUInt(unsigned int& i, VARIANT& var, unsigned int iDefault);
inline HRESULT GetOptionalULong(unsigned long& i, VARIANT& var, unsigned long iDefault)
{
	ASSERT(sizeof(unsigned long)==sizeof(unsigned int));
	return GetOptionalUInt((unsigned int&)i, var, (unsigned int)iDefault);
}
HRESULT GetOptionalObject(VARIANT& var, REFIID riid, void** ppvObject);
HRESULT GetOptionalDouble(double& dbl, VARIANT& var, double dblDefault);


}	// namespace Simple

#endif	// __VARIANTPARAMS_H

