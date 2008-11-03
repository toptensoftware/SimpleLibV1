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
// CryptorStream.cpp - implementation of CryptorStream

#include "stdafx.h"
#include "SimpleLibWinBuild.h"
#include "StreamHelpers.h"

#include "CryptorStream.h"

namespace Simple
{


// Open a stream on a fixed block of memory	 
HRESULT SIMPLEAPI CreateCryptorStream(IStream* pStreamUnderlying, const CCryptorKey& Key, IStream** pVal)
{
	// Allocate new stream object
	CCryptorStream* pStream=new CCryptorStream();

	// Initialise it..
	pStream->Init(pStreamUnderlying, Key);

	*pVal=pStream;
	return S_OK;
}


// Constructor
CCryptorStream::CCryptorStream()
{
	m_dwRef=1;
}

// Destructor
CCryptorStream::~CCryptorStream()
{
}

// Initialisation
HRESULT CCryptorStream::Init(IStream* pStreamUnderlying, const CCryptorKey& key)
{
	m_Cryptor.SetKey(key);
	m_Key=key;
	m_spUnderlyingStream=pStreamUnderlying;
	return S_OK;
}

STDMETHODIMP_(ULONG) CCryptorStream::AddRef()
{
	return m_dwRef++;
}

STDMETHODIMP_(ULONG) CCryptorStream::Release()
{
	m_dwRef--;

	if (!m_dwRef)
		{
		delete this;
		return 0;
		}

	return m_dwRef;
}

STDMETHODIMP CCryptorStream::QueryInterface(REFIID riid, void** ppvObject)
{
	if (riid==IID_IUnknown || riid==IID_IStream || riid==IID_ISequentialStream)
		{
		*ppvObject=static_cast<IStream*>(this);
		AddRef();
		return S_OK;
		}

	return E_NOINTERFACE;
}

// Implementation of Read
STDMETHODIMP CCryptorStream::Read(void* pv, ULONG cb, ULONG* pcbRead)
{
	ULONG ulTemp;
	if (!pcbRead)
	{
		pcbRead=&ulTemp;
	}

	ASSERT(m_Cryptor.GetOffset()==GetStreamOffset(m_spUnderlyingStream));

	// Read source data
	HRESULT hr=m_spUnderlyingStream->Read(pv, cb, pcbRead);

	// Decrypt it
	m_Cryptor.TransformBuffer(pv, pcbRead[0]);

	return hr;
}

// Implementation of Write
STDMETHODIMP CCryptorStream::Write(void const* pv, ULONG cb, ULONG* pcbWritten)
{
	ASSERT(m_Cryptor.GetOffset()==GetStreamOffset(m_spUnderlyingStream));

	// Encrypt incoming buffer
	void* pTemp=NULL;
	if (cb)
	{
		pTemp=malloc(cb);
		memcpy(pTemp, pv, cb);

		m_Cryptor.TransformBuffer(pTemp, cb);
	}

	// Write to underlying stream
	ULONG ulTemp;
	if (!pcbWritten)
	{
		pcbWritten=&ulTemp;
	}
	HRESULT hr=m_spUnderlyingStream->Write(pTemp, cb, pcbWritten);

	if (pcbWritten[0]!=cb)
	{
		m_Cryptor.Seek(GetStreamOffset(m_spUnderlyingStream));
	}

	if (pTemp)
		free(pTemp);

	return hr;
}

// Implementation of Seek
STDMETHODIMP CCryptorStream::Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER* plibNewPosition)
{
	// Seek
	HRESULT hr=m_spUnderlyingStream->Seek(dlibMove, dwOrigin, plibNewPosition);

	// Move to correct position
	m_Cryptor.Seek(GetStreamOffset(m_spUnderlyingStream));

	return hr;
}

// Implementation of SetSize
STDMETHODIMP CCryptorStream::SetSize(ULARGE_INTEGER libNewSize)
{
	return E_NOTIMPL;
}

// Implementation of CopyTo
STDMETHODIMP CCryptorStream::CopyTo(IStream* pstm, ULARGE_INTEGER cb, ULARGE_INTEGER* pcbRead, ULARGE_INTEGER* pcbWritten)
{
	return E_NOTIMPL;
}

// Implementation of Commit
STDMETHODIMP CCryptorStream::Commit(DWORD grfCommitFlags)
{
	return S_OK;
}

// Implementation of Revert
STDMETHODIMP CCryptorStream::Revert()
{
	return E_NOTIMPL;
}

// Implementation of LockRegion
STDMETHODIMP CCryptorStream::LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
{
	return E_NOTIMPL;
}

// Implementation of UnlockRegion
STDMETHODIMP CCryptorStream::UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
{
	return E_NOTIMPL;
}

// Implementation of Stat
STDMETHODIMP CCryptorStream::Stat(STATSTG* pstatstg, DWORD grfStatFlag)
{
	return m_spUnderlyingStream->Stat(pstatstg, grfStatFlag);
}

// Implementation of Clone
STDMETHODIMP CCryptorStream::Clone(IStream** ppstm)
{
	return CreateCryptorStream(m_spUnderlyingStream, m_Key, ppstm);
}


}