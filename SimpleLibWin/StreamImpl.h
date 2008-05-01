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
// StreamImpl.h - declaration of StreamImpl wrappers

#ifndef __STREAMIMPL_H
#define __STREAMIMPL_H

namespace Simple
{

// Implementation of IStream on a Win 32 file handle
template <class T>
class CRefCountedStream : public T
{
public:
// Construction
	CRefCountedStream()
	{
		m_dwRef=0;
	};

	virtual ~CRefCountedStream()
	{
	}

// IUnknown
	STDMETHODIMP_(ULONG) AddRef()
	{
		m_dwRef++;
		return m_dwRef;
	}

	STDMETHODIMP_(ULONG) Release()
	{
		ASSERT(m_dwRef);
		m_dwRef--;
		if (m_dwRef==0)
		{
			delete this;
			return 0;
		}

		return m_dwRef;
	}

	STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject)
	{
		if (riid==IID_IUnknown || riid==IID_IStream || riid==IID_ISequentialStream)
			{
			*ppvObject=static_cast<IStream*>(this);
			AddRef();
			return S_OK;
			}

		return E_NOINTERFACE;
	}

	DWORD m_dwRef;
};

// Implementation of IStream on a Win 32 file handle
template <class T>
class CInstanceStream : public T
{
public:
// Construction
	CInstanceStream()
	{
	};

	virtual ~CInstanceStream()
	{
	}

// IUnknown
	STDMETHODIMP_(ULONG) AddRef()
	{
		return 2;
	}

	STDMETHODIMP_(ULONG) Release()
	{
		return 1;
	}

	STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject)
	{
		if (riid==IID_IUnknown || riid==IID_IStream || riid==IID_ISequentialStream)
			{
			*ppvObject=static_cast<IStream*>(this);
			AddRef();
			return S_OK;
			}

		return E_NOINTERFACE;
	}
};



}	// namespace Simple

#endif	// __STREAMIMPL_H


