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
// SubStream.cpp - implementation of SubStream

#include "stdafx.h"
#include "SimpleLibWinBuild.h"

#include "SubStream.h"

#include "StreamHelpers.h"

namespace Simple
{
/////////////////////////////////////////////////////////////////////////////
// CSubStream

// Constructor for write
CSubStream::CSubStream()
{
	m_dwPos=0;
}

// Destructor
CSubStream::~CSubStream()
{
}

// Initialise this substream
HRESULT CSubStream::Init(IStream* pUnderlying, bool bReadOnly, DWORD dwOffset, DWORD dwLength)
{
	m_spUnderlying=pUnderlying;
	m_dwOffset=dwOffset;
	m_dwLength=dwLength;
	m_bReadOnly=bReadOnly;
	m_dwPos=0;
	return S_OK;
}

HRESULT CSubStream::Close()
{
	m_spUnderlying.Release();
	m_dwOffset=0;
	m_dwPos=0;
	m_dwLength=0;
	return S_OK;
}



// Read from this substream
STDMETHODIMP CSubStream::Read(void* pv, ULONG cb, ULONG* pcbRead)
{
	// Zombie check
	ASSERT(m_spUnderlying!=NULL);
	if (!m_spUnderlying)
		return E_UNEXPECTED;

	// If past limit, don't read anything
	if (m_dwPos>=m_dwLength)
	{
		*pcbRead=0;
		return S_FALSE;
	}

	// Clip read length to available
	if (m_dwPos+cb > m_dwLength)
	{
		cb=m_dwLength-m_dwPos;
	}

	// Handle NULL
	ULONG cbRead;
	if (!pcbRead)
		pcbRead=&cbRead;

	// Read from underlying stream
	StreamSeek(m_spUnderlying, m_dwOffset+m_dwPos);
	HRESULT hr=m_spUnderlying->Read(pv, cb, pcbRead);

	// Update current position
	m_dwPos+=*pcbRead;

	return hr;


}

// Write to underlying sub stream
STDMETHODIMP CSubStream::Write(void const* pv, ULONG cb, ULONG* pcbWritten)
{
	// Readonly?
	if (m_bReadOnly)
	{
		ASSERT(FALSE);
		return E_ACCESSDENIED;
	}

	// Zombie check
	ASSERT(m_spUnderlying!=NULL);
	if (!m_spUnderlying)
		return E_UNEXPECTED;

	// Handle NULL
	ULONG cbWritten;
	if (!pcbWritten)
		pcbWritten=&cbWritten;

	// Write to underlying stream
	StreamSeek(m_spUnderlying, m_dwPos+m_dwOffset);
	HRESULT hr=m_spUnderlying->Write(pv, cb, pcbWritten);

	// Update current position
	m_dwPos+=*pcbWritten;
	
	// Update length
	if (m_dwPos > m_dwLength)
		m_dwLength=m_dwPos;

	// Done
	return hr;
}

// Seek
STDMETHODIMP CSubStream::Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER* plibNewPosition)
{
	// Zombie check
	ASSERT(m_spUnderlying!=NULL);
	if (!m_spUnderlying)
		return E_UNEXPECTED;

	// Work out how much to move by
	int iOffset=(int)dlibMove.QuadPart;


	switch (dwOrigin)
	{
		case STREAM_SEEK_CUR:
		{
			if (iOffset<0 || ((DWORD)-iOffset) > m_dwPos)
				m_dwPos=0;
			else
				m_dwPos+=iOffset;
			break;
		}

		case STREAM_SEEK_SET:
			m_dwPos=(DWORD)iOffset;
			break;

		case STREAM_SEEK_END:
			if (iOffset<0 || ((DWORD)-iOffset) > m_dwLength)
				m_dwPos=0;
			else
				m_dwPos=m_dwLength+iOffset;
			break;
	}

	// Return new position
	if (plibNewPosition)
	{
		plibNewPosition->QuadPart=m_dwPos;
	}

	return S_OK;
}

STDMETHODIMP CSubStream::SetSize(ULARGE_INTEGER libNewSize)
{
	return E_NOTIMPL;
}

STDMETHODIMP CSubStream::CopyTo(IStream* pstm, ULARGE_INTEGER cb, ULARGE_INTEGER* pcbRead, ULARGE_INTEGER* pcbWritten)
{
	return E_NOTIMPL;
}

STDMETHODIMP CSubStream::Commit(DWORD grfCommitFlags)
{
	return S_OK;
}

STDMETHODIMP CSubStream::Revert()
{
	return E_NOTIMPL;
}

STDMETHODIMP CSubStream::LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
{
	return E_NOTIMPL;
}

STDMETHODIMP CSubStream::UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
{	
	return E_NOTIMPL;
}

STDMETHODIMP CSubStream::Stat(STATSTG* pstatstg, DWORD grfStatFlag)
{
	return E_NOTIMPL;
}

STDMETHODIMP CSubStream::Clone(IStream** ppstm)
{
	return E_NOTIMPL;
}




}	// namespace Simple
