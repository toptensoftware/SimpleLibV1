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
// SubStream.h - declaration of SubStream

#ifndef __SUBSTREAM_H
#define __SUBSTREAM_H

namespace Simple
{

class CSubStream : public IStream
{
public:
// Constructor
			CSubStream();
	virtual ~CSubStream();

// Initialisation
	HRESULT Init(IStream* pUnderlying, bool bReadOnly, DWORD dwOffset, DWORD dwLength);
	HRESULT Close();

// IStream
	STDMETHODIMP Read(void* pv, ULONG cb, ULONG* pcbRead);
	STDMETHODIMP Write(void const* pv, ULONG cb, ULONG* pcbWritten);
	STDMETHODIMP Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER* plibNewPosition);
	STDMETHODIMP SetSize(ULARGE_INTEGER libNewSize);
	STDMETHODIMP CopyTo(IStream* pstm, ULARGE_INTEGER cb, ULARGE_INTEGER* pcbRead, ULARGE_INTEGER* pcbWritten);
	STDMETHODIMP Commit(DWORD grfCommitFlags);
	STDMETHODIMP Revert();
	STDMETHODIMP LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType);
	STDMETHODIMP UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType);
	STDMETHODIMP Stat(STATSTG* pstatstg, DWORD grfStatFlag);
	STDMETHODIMP Clone(IStream** ppstm);

	CAutoPtr<IStream, SRefCounted> m_spUnderlying;
	bool	m_bReadOnly;
	DWORD m_dwOffset;
	DWORD m_dwLength;
	DWORD m_dwPos;
};




}	// namespace Simple

#endif	// __SUBSTREAM_H


