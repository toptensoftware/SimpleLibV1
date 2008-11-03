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
// CryptorStream.h - declaration of CryptorStream

#ifndef __CRYPTORSTREAM_H
#define __CRYPTORSTREAM_H


namespace Simple
{

class CCryptorStream : 
	public IStream
{
public:
// Construction
			CCryptorStream();
	virtual ~CCryptorStream();

// Initialisation
	HRESULT Init(IStream* pStreamUnderlying, const CCryptorKey& key);

// Attributes
	CAutoPtr<IStream, SRefCounted>	m_spUnderlyingStream;
	DWORD				m_dwRef;
	CCryptor			m_Cryptor;
	CCryptorKey			m_Key;


// IUnknown
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();
	STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject);

// IStreamSequential
	STDMETHODIMP Read(void* pv, ULONG cb, ULONG* pcbRead);
	STDMETHODIMP Write(void const* pv, ULONG cb, ULONG* pcbWritten);

// IStream
	STDMETHODIMP Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER* plibNewPosition);
	STDMETHODIMP SetSize(ULARGE_INTEGER libNewSize);
	STDMETHODIMP CopyTo(IStream* pstm, ULARGE_INTEGER cb, ULARGE_INTEGER* pcbRead, ULARGE_INTEGER* pcbWritten);
	STDMETHODIMP Commit(DWORD grfCommitFlags);
	STDMETHODIMP Revert();
	STDMETHODIMP LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType);
	STDMETHODIMP UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType);
	STDMETHODIMP Stat(STATSTG* pstatstg, DWORD grfStatFlag);
	STDMETHODIMP Clone(IStream** ppstm);

};


HRESULT SIMPLEAPI CreateCryptorStream(IStream* pStreamUnderlying, const CCryptorKey& key, IStream** pVal);

}	// namespace Simple

#endif	// __CRYPTORSTREAM_H

