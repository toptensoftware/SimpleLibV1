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
// SimpleMemoryStream.h - declaration of SimpleMemoryStream

#ifndef __SIMPLEMEMORYSTREAM_H
#define __SIMPLEMEMORYSTREAM_H


namespace Simple
{

class CSimpleMemoryStream : 
	public IStream
{
public:
// Construction
			CSimpleMemoryStream();
	virtual ~CSimpleMemoryStream();

// Initialisation
	HRESULT Init(LPVOID pData, UINT cbData);

// Attributes
	LPBYTE		m_pData;
	LPBYTE		m_pPosition;
	LPBYTE		m_pEndPosition;
	DWORD		m_dwRef;


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


HRESULT SIMPLEAPI OpenMemoryStream(LPVOID pData, DWORD cbData, IStream** pVal);
HRESULT SIMPLEAPI OpenResourceStream(HINSTANCE hInstance, const wchar_t* pszName, const wchar_t* pszType, IStream** pVal);

}	// namespace Simple

#endif	// __SIMPLEMEMORYSTREAM_H

