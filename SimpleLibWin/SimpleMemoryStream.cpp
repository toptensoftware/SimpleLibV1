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
// SimpleMemoryStream.cpp - implementation of SimpleMemoryStream

#include "stdafx.h"
#include "SimpleLibWinBuild.h"

#include "SimpleMemoryStream.h"

namespace Simple
{



// Open a stream on a fixed block of memory
HRESULT OpenMemoryStream(LPVOID pData, DWORD cbData, IStream** pVal)
{
	// Allocate new stream object
	CSimpleMemoryStream* pStream=new CSimpleMemoryStream();

	// Initialise it..
	pStream->Init(pData, cbData);

	*pVal=pStream;
	return S_OK;
}

// Open a stream on an embedded resource
HRESULT OpenResourceStream(HINSTANCE hInstance, const wchar_t* pszName, const wchar_t* pszType, IStream** pVal)
{
	// Find it
	HRSRC hRsrc=FindResource(hInstance, pszName, pszType);
	if (!hRsrc)
		return HRESULT_FROM_WIN32(GetLastError());

	// Load resource
	HGLOBAL hData=LoadResource(hInstance, hRsrc);
	if (!hData)
		return HRESULT_FROM_WIN32(GetLastError());

	// Get its size
	DWORD dwSize=SizeofResource(hInstance, hRsrc);
	if (!dwSize)
		return HRESULT_FROM_WIN32(GetLastError());

	// Open it
	return OpenMemoryStream(LockResource(hData), dwSize, pVal);
}


// Constructor
CSimpleMemoryStream::CSimpleMemoryStream()
{
	m_pData=NULL;
	m_pPosition=NULL;
	m_pEndPosition=NULL;
	m_dwRef=1;
}

// Destructor
CSimpleMemoryStream::~CSimpleMemoryStream()
{
}

// Initialisation
HRESULT CSimpleMemoryStream::Init(LPVOID pData, UINT cbData)
{
	m_pData=(LPBYTE)pData;
	m_pPosition=m_pData;
	m_pEndPosition=m_pData + cbData;
	return S_OK;
}

STDMETHODIMP_(ULONG) CSimpleMemoryStream::AddRef()
{
	return m_dwRef++;
}

STDMETHODIMP_(ULONG) CSimpleMemoryStream::Release()
{
	m_dwRef--;

	if (!m_dwRef)
		{
		delete this;
		return 0;
		}

	return m_dwRef;
}

STDMETHODIMP CSimpleMemoryStream::QueryInterface(REFIID riid, void** ppvObject)
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
STDMETHODIMP CSimpleMemoryStream::Read(void* pv, ULONG cb, ULONG* pcbRead)
{
	// Special optimized case for 1 byte read (for efficient loading of bini from resource)
	if (cb==1 && m_pPosition<m_pEndPosition)
	{
		*(BYTE*)pv=*m_pPosition++;
		if (pcbRead)
			*pcbRead=1;
		return S_OK;
	}

	// Handle NULL out pointer
	ULONG cbRead;
	if (!pcbRead) pcbRead=&cbRead;

	// Past end of data?
	if (!cb || m_pPosition >= m_pEndPosition)
		{
		*pcbRead=0;
		return S_FALSE;
		}

	// Read overlapping end of data?
	if (m_pPosition + cb > m_pEndPosition)
	{
		cb=(ULONG)(m_pEndPosition-m_pPosition);
	}

	memcpy(pv, m_pPosition, cb);

	// Update position
	m_pPosition+=cb;

	// Return amount read
	*pcbRead=cb;
	return cb>0 ? S_OK : S_FALSE;
}

// Implementation of Write
STDMETHODIMP CSimpleMemoryStream::Write(void const* pv, ULONG cb, ULONG* pcbWritten)
{
	return E_NOTIMPL;
}

// Implementation of Seek
STDMETHODIMP CSimpleMemoryStream::Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER* plibNewPosition)
{
	switch (dwOrigin)
		{
		case STREAM_SEEK_SET:
			m_pPosition = m_pData + (UINT)(dlibMove.QuadPart);
			break;

		case STREAM_SEEK_CUR:
			m_pPosition = m_pPosition + dlibMove.QuadPart;
			break;

		case STREAM_SEEK_END:
			m_pPosition = m_pEndPosition - (UINT)(dlibMove.QuadPart);
			break;

		default:
			return E_INVALIDARG;
		}

	if (plibNewPosition)
		plibNewPosition->QuadPart=m_pPosition-m_pData;

	return S_OK;
}

// Implementation of SetSize
STDMETHODIMP CSimpleMemoryStream::SetSize(ULARGE_INTEGER libNewSize)
{
	return E_NOTIMPL;
}

// Implementation of CopyTo
STDMETHODIMP CSimpleMemoryStream::CopyTo(IStream* pstm, ULARGE_INTEGER cb, ULARGE_INTEGER* pcbRead, ULARGE_INTEGER* pcbWritten)
{
	return E_NOTIMPL;
}

// Implementation of Commit
STDMETHODIMP CSimpleMemoryStream::Commit(DWORD grfCommitFlags)
{
	return S_OK;
}

// Implementation of Revert
STDMETHODIMP CSimpleMemoryStream::Revert()
{
	return E_NOTIMPL;
}

// Implementation of LockRegion
STDMETHODIMP CSimpleMemoryStream::LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
{
	return E_NOTIMPL;
}

// Implementation of UnlockRegion
STDMETHODIMP CSimpleMemoryStream::UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
{
	return E_NOTIMPL;
}

// Implementation of Stat
STDMETHODIMP CSimpleMemoryStream::Stat(STATSTG* pstatstg, DWORD grfStatFlag)
{
	memset(pstatstg, 0, sizeof(STATSTG));
	pstatstg->type=STGTY_STREAM;
	pstatstg->cbSize.QuadPart=m_pEndPosition-m_pData;
	return S_OK;
}

// Implementation of Clone
STDMETHODIMP CSimpleMemoryStream::Clone(IStream** ppstm)
{
	return OpenMemoryStream(m_pData, (UINT)(m_pEndPosition-m_pData), ppstm);
}






}