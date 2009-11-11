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
// File.h - declaration of IFile class

#ifndef __FILE_H
#define __FILE_H

#include "Result.h"

namespace Simple
{

// Seek origin
enum FileSeekOrigin
{
	OriginSet=SEEK_SET,
	OriginStart=SEEK_SET,
	OriginEnd=SEEK_END,
	OriginCurrent=SEEK_CUR,
};

// IFile interface
struct IFile
{
public:
// IFile
	virtual result_t Read(void* pBuf, uint32_t cb, uint32_t* pcb)=0;
	virtual result_t Write(const void* pBuf, uint32_t cb, uint32_t* pcb)=0;
	virtual result_t Seek(int64_t offset, FileSeekOrigin origin=OriginStart, uint64_t* pcbPos=NULL)=0;
	virtual result_t Truncate()=0;


	result_t GetLength(uint64_t* pcbLen)
	{
		uint64_t iPos;
		ReturnIfFailed(GetCurrentPosition(&iPos));
		ReturnIfFailed(Seek(0, OriginEnd, pcbLen));
		return Seek(iPos);
	}
	uint64_t GetLength()
	{
		uint64_t len=0;
		GetLength(&len);
		return len;
	}
	result_t GetCurrentPosition(uint64_t* pcbPos)
	{
		return Seek(0, OriginCurrent, pcbPos);
	}

	uint64_t GetCurrentPosition()
	{
		uint64_t pos=0;
		GetCurrentPosition(&pos);
		return pos;
	}

};


// Flags to CFile::Open
enum FileFlag
{
	ffRead			=0x0001,
	ffWrite			=0x0002,
	ffCreate		=0x0004,
	ffOpen			=0x0008,
	ffAppend		=0x0010,
	ffShareRead		=0x0100,
	ffShareWrite	=0x0200,
};

// Implementation for actual filesystem file
class CFile : public IFile
{
public:
// Construction
			CFile();
	virtual ~CFile();

// Operations							 
	result_t Open(const CAnyString& pszFileName, uint32_t flags);
	result_t OpenExisting(const CAnyString& pszFileName)
		{ return Open(pszFileName, ffRead|ffOpen); }
	result_t CreateNew(const CAnyString& pszFileName) 
		{ return Open(pszFileName, ffWrite|ffCreate); }
	result_t Append(const CAnyString& pszFileName)
		{ return Open(pszFileName, ffAppend|ffOpen|ffCreate); }
	void Close();

// IFile
	virtual result_t Read(void* pBuf, uint32_t cb, uint32_t* pcb);
	virtual result_t Write(const void* pBuf, uint32_t cb, uint32_t* pcb);
	virtual result_t Seek(int64_t offset, FileSeekOrigin origin, uint64_t* pPos);
	virtual result_t Truncate();

// Implementation
protected:
// Attributes
	FILE*	m_pFile;

};

}

#endif	// __FILE_H

