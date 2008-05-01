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
// SafeArray.h - declaration of CSafeArray

#ifndef __SAFEARRAY_H
#define __SAFEARRAY_H

namespace Simple
{

class CSafeArray
{
public:
	CSafeArray();
	virtual ~CSafeArray();

	void Release();
	SAFEARRAY* Detach();
	void Attach(SAFEARRAY* pArray);
	SAFEARRAY** operator&();

	operator SAFEARRAY*()
	{
		return m_pArray;
	}

	UINT GetDim();
	long GetLBound(UINT nDim=1);
	long GetUBound(UINT nDim=1);
	long GetElements(UINT nDim=1);
	SAFEARRAY*	m_pArray; 
};



}	// namespace Simple

#endif		// __SAFEARRAY_H

