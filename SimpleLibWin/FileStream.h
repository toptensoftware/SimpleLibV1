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
// FileStream.h - declaration of FileStream

#ifndef __FILESTREAM_H
#define __FILESTREAM_H

namespace Simple
{

class IFileStreamImpl : public IStream
{
public:
// Construction
	IFileStreamImpl(HANDLE hFile=NULL);
	virtual ~IFileStreamImpl();

	BOOL Create(LPCTSTR pszFileName);
	BOOL Open(LPCTSTR pszFileName, DWORD dwShare=FILE_SHARE_READ);
	BOOL OpenReadWrite(LPCTSTR pszFileName);
	void Close();
	bool IsOpen();
	HANDLE Detach();
	void Attach(HANDLE hFile);
	DWORD Seek(LONG lMove, DWORD dwOrigin);

// Attributes
	HANDLE m_hFile;

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
};

// Implementation of IStream on a Win 32 file handle
class CFileStream : public IFileStreamImpl
{
public:
// Construction
	CFileStream(HANDLE hFile=NULL) : 
		IFileStreamImpl(hFile)
	{
	};

	~CFileStream()
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

#endif	// __FILESTREAM_H


