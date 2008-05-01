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
// SafeArray.cpp - implementation of CSafeArray

#include "stdafx.h"
#include "SimpleLibAtlComBuild.h"

#include "SafeArray.h"


namespace Simple
{

CSafeArray::CSafeArray()
{
	m_pArray=NULL;
};

CSafeArray::~CSafeArray()
{
	Release();
}

void CSafeArray::Release()
{
	if (m_pArray)
		{
		SafeArrayDestroy(m_pArray);
		m_pArray=NULL;
		}
}

SAFEARRAY* CSafeArray::Detach()
{
	SAFEARRAY* pRet=m_pArray;
	m_pArray=NULL;
	return pRet;
}

void CSafeArray::Attach(SAFEARRAY* pArray)
{
	ASSERT(m_pArray==NULL);
	m_pArray=pArray;
}

SAFEARRAY** CSafeArray::operator&()
{
	ASSERT(m_pArray==NULL);
	return &m_pArray;
};

UINT CSafeArray::GetDim()
{
	ASSERT(m_pArray!=NULL);
	return SafeArrayGetDim(m_pArray);
}

long CSafeArray::GetLBound(UINT nDim)
{
	ASSERT(m_pArray!=NULL);
	long lRetv=-1;
	HRESULT hr=SafeArrayGetLBound(m_pArray, nDim, &lRetv);
	ASSERT(SUCCEEDED(hr));
	return lRetv;
}

long CSafeArray::GetUBound(UINT nDim)
{
	ASSERT(m_pArray!=NULL);
	long lRetv=-1;
	HRESULT hr=SafeArrayGetUBound(m_pArray, nDim, &lRetv);
	ASSERT(SUCCEEDED(hr));
	return lRetv;
}

long CSafeArray::GetElements(UINT nDim)
{
	return GetUBound(nDim)-GetLBound(nDim)+1;
}



}	// namespace Simple
