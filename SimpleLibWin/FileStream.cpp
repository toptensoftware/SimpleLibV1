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
// FileStream.cpp - implementation of FileStream

#include "stdafx.h"
#include "SimpleLibWinBuild.h"

#include "FileStream.h"



namespace Simple
{


IFileStreamImpl::IFileStreamImpl(HANDLE hFile)
{
	m_hFile=hFile;
}

IFileStreamImpl::~IFileStreamImpl()
{
	Close();
}

// Create a brand spanking new file...
BOOL IFileStreamImpl::Create(LPCTSTR pszFileName)
{
	m_hFile=CreateFile(pszFileName, GENERIC_WRITE|GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	return m_hFile!=INVALID_HANDLE_VALUE;
}

// Open an existing file
BOOL IFileStreamImpl::Open(LPCTSTR pszFileName)
{
	m_hFile=CreateFile(pszFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	return m_hFile!=INVALID_HANDLE_VALUE;
}

// Open an existing file
BOOL IFileStreamImpl::OpenReadWrite(LPCTSTR pszFileName)
{
	m_hFile=CreateFile(pszFileName, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	return m_hFile!=INVALID_HANDLE_VALUE;
}

void IFileStreamImpl::Close()
{
	if (m_hFile)
		{
		CloseHandle(m_hFile);
		m_hFile=NULL;
		}
}

bool IFileStreamImpl::IsOpen() 
{ 
	return m_hFile!=NULL; 
};

// Detach file handle
HANDLE IFileStreamImpl::Detach()
{
	HANDLE hRet=m_hFile;
	m_hFile=NULL;
	return hRet;
};

// Attach file handle
void IFileStreamImpl::Attach(HANDLE hFile)
{
	m_hFile=hFile;
}

// Easier version of seek
DWORD IFileStreamImpl::Seek(LONG lMove, DWORD dwOrigin)
{
	return SetFilePointer(m_hFile, lMove, NULL, dwOrigin);
}

// IStream
STDMETHODIMP IFileStreamImpl::Read(void* pv, ULONG cb, ULONG* pcbRead)
{
	ULONG ulTemp;
	if (!pcbRead)
		pcbRead=&ulTemp;
	if (ReadFile(m_hFile, pv, cb, pcbRead, NULL))
		return S_OK;
	return S_FALSE;
};

STDMETHODIMP IFileStreamImpl::Write(void const* pv, ULONG cb, ULONG* pcbWritten)
{
	ULONG ulTemp;
	if (!pcbWritten)
		pcbWritten=&ulTemp;
	if (WriteFile(m_hFile, pv, cb, pcbWritten, NULL))
		return S_OK;
	return S_FALSE;
};

STDMETHODIMP IFileStreamImpl::Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER* plibNewPosition)
{
	ULARGE_INTEGER ulTemp;
	if (!plibNewPosition)
		plibNewPosition=&ulTemp;
	plibNewPosition->QuadPart=0;

	if (dlibMove.HighPart)
		return E_NOTIMPL;

	plibNewPosition->LowPart=SetFilePointer(m_hFile, dlibMove.LowPart, (long*)&plibNewPosition->HighPart, dwOrigin);
	return S_OK;
};

STDMETHODIMP IFileStreamImpl::SetSize(ULARGE_INTEGER libNewSize)
{
	if (libNewSize.HighPart)
		return E_NOTIMPL;

	// Save old position
	DWORD dwOldPos=Seek(0, STREAM_SEEK_CUR);
	if (dwOldPos==INVALID_SET_FILE_POINTER)
		return HRESULT_FROM_WIN32(GetLastError());

	// Seek to new length
	if (Seek(libNewSize.LowPart, FILE_BEGIN)==INVALID_SET_FILE_POINTER)	
		return HRESULT_FROM_WIN32(GetLastError());

	// Truncate/extend
	HRESULT hr=S_OK;
	if (!SetEndOfFile(m_hFile))
		{
		hr=HRESULT_FROM_WIN32(GetLastError());
		}

	// Seek back to current position
	Seek(dwOldPos, STREAM_SEEK_SET);

	return hr;
};

STDMETHODIMP IFileStreamImpl::CopyTo(IStream* pstm, ULARGE_INTEGER cb, ULARGE_INTEGER* pcbRead, ULARGE_INTEGER* pcbWritten)
{
	return E_NOTIMPL;
};

STDMETHODIMP IFileStreamImpl::Commit(DWORD grfCommitFlags)
{
	if (!FlushFileBuffers(m_hFile))
		HRESULT_FROM_WIN32(GetLastError());

	return S_OK;
};

STDMETHODIMP IFileStreamImpl::Revert()
{
	return E_NOTIMPL;
};

STDMETHODIMP IFileStreamImpl::LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
{
	return E_NOTIMPL;
};

STDMETHODIMP IFileStreamImpl::UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
{
	return E_NOTIMPL;
};

STDMETHODIMP IFileStreamImpl::Stat(STATSTG* pstatstg, DWORD grfStatFlag)
{
	pstatstg->pwcsName=NULL;
	pstatstg->type=STGTY_STREAM;
	pstatstg->cbSize.LowPart=GetFileSize(m_hFile, &pstatstg->cbSize.HighPart);
	GetFileTime(m_hFile, &pstatstg->ctime, &pstatstg->atime, &pstatstg->mtime);
	pstatstg->grfMode=0;
	pstatstg->grfLocksSupported=0;
	pstatstg->clsid=CLSID_NULL;
	pstatstg->grfStateBits=0;
	pstatstg->reserved=0;
	return S_OK;
};

STDMETHODIMP IFileStreamImpl::Clone(IStream** ppstm)
{
	return E_NOTIMPL;
};


}	// namespace Simple
