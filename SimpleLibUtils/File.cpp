//////////////////////////////////////////////////////////////////////
//
// SimpleLib Utils Version 1.0
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
// File.cpp - implementation of IFile class

#include "stdafx.h"
#include "SimpleLibUtilsBuild.h"

#include "File.h"
#include "PathLibrary.h"

#include <share.h>

namespace Simple
{

//////////////////////////////////////////////////////////////////////////
// CFile

// Constructor
CFile::CFile()
{
	m_pFile=NULL;
}

// Destructor
CFile::~CFile()
{
	Close();
}

// Open a file
result_t CFile::Open(const CAnyString& pszFileName, uint32_t flags)
{
	Close();

	// If create or open mode specified, select depending on file existance
	if ((flags & ffCreate) && (flags & ffOpen))
	{
		if (DoesFileExist(pszFileName))
			flags &= ~ffCreate;
		else
			flags &= ~ffOpen;
	}

	// Work out fopen flags
	const wchar_t* pszFlags=NULL;
	switch (flags & 0x000F)
	{
		case ffWrite|ffCreate:
			pszFlags=L"wb";
			break;

		case ffRead|ffOpen:
			pszFlags=L"rb";
			break;

		case ffRead|ffWrite|ffCreate:
			pszFlags=L"wb+";
			break;

		case ffRead|ffWrite|ffOpen:
			pszFlags=L"rb+";
			break;

		default:
			return e_invalidarg;
	}

	// Work out sharing flags
	int shareflag=_SH_DENYRW;
	switch (flags & (ffShareRead|ffShareWrite))
	{
		case ffShareRead|ffShareWrite:
			shareflag=_SH_DENYNO;
			break;

		case ffShareRead:
			shareflag=_SH_DENYWR;
			break;

		case ffShareWrite:
			shareflag=_SH_DENYRD;
			break;
	}
	
	// Open file
	m_pFile=_wfsopen(pszFileName, pszFlags, shareflag);
	if (!m_pFile)
		return ResultFromErrno(errno);

	// Append?
	if ((flags & ffAppend) && (flags & ffOpen))
	{
		_fseeki64(m_pFile, 0, SEEK_END);
	}
	
	// Done
	return s_ok;
}

void CFile::Close()
{
	if (m_pFile)
	{
		fclose(m_pFile);
		m_pFile=NULL;
	}
}

// Implementation of Read
result_t CFile::Read(void* pBuf, uint32_t cb, uint32_t* pcb)
{
	ASSERT(m_pFile!=NULL);

	uint32_t temp;
	temp=(uint32_t)fread(pBuf, 1, cb, m_pFile);
	if (pcb)
		pcb[0]=temp;

	if (temp==cb)
		return 0;

	errno_t err=errno;

	if (feof(m_pFile))
	{
		// If eof and didn't ask for return count, convert to error
		return pcb==NULL ? e_eof : 0;
	}

	return ResultFromErrno(err);
}

// Implementation of Write
result_t CFile::Write(const void* pBuf, uint32_t cb, uint32_t* pcb)
{
	ASSERT(m_pFile!=NULL);

	uint32_t temp;
	temp=(uint32_t)fwrite(pBuf, 1, cb, m_pFile);
	if (pcb)
		pcb[0]=temp;

	if (temp==cb)
		return 0;

	return ResultFromErrno(errno);
}

// Implementation of Seek
result_t CFile::Seek(int64_t offset, FileSeekOrigin origin, uint64_t* pcb)
{
	if (_fseeki64(m_pFile, offset, origin)!=0)
		return ResultFromErrno(errno);

	if (pcb)
		*pcb=_ftelli64(m_pFile);

	return 0;
}

// Implementation of SetSize
result_t CFile::Truncate()
{
	ASSERT(false);
	return e_notimpl;
}

}