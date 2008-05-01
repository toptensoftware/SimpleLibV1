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
BOOL SIMPLEAPI IsParameterSpecified(VARIANT& var);

// Convert a variant to either an integer or a string
HRESULT SIMPLEAPI VariantToStringOrInt(VARIANT& Item, int& i, CComBSTR& bstr);
HRESULT SIMPLEAPI VariantToStringOrIntOrObject(VARIANT& Item, int* pi, BSTR* pbstrVal, IUnknown** pVal);

HRESULT SIMPLEAPI GetOptionalString(CComBSTR& bstr, VARIANT& var, LPCOLESTR pszDefault);
HRESULT SIMPLEAPI GetOptionalShort(short& i, VARIANT& var, short iDefault);
HRESULT SIMPLEAPI GetOptionalUShort(unsigned short& i, VARIANT& var, unsigned short iDefault);
HRESULT SIMPLEAPI GetOptionalInt(int& i, VARIANT& var, int iDefault);
HRESULT SIMPLEAPI GetOptionalUInt(unsigned int& i, VARIANT& var, unsigned int iDefault);
inline HRESULT SIMPLEAPI GetOptionalULong(unsigned long& i, VARIANT& var, unsigned long iDefault)
{
	ASSERT(sizeof(unsigned long)==sizeof(unsigned int));
	return GetOptionalUInt((unsigned int&)i, var, (unsigned int)iDefault);
}
HRESULT SIMPLEAPI GetOptionalObject(VARIANT& var, REFIID riid, void** ppvObject);
HRESULT SIMPLEAPI GetOptionalDouble(double& dbl, VARIANT& var, double dblDefault);


}	// namespace Simple

#endif	// __VARIANTPARAMS_H

