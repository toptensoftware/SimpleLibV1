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
// StreamHelpers.cpp - implementation of StreamHelpers

#include "stdafx.h"
#include "SimpleLibWinBuild.h"

#include "StreamHelpers.h"



namespace Simple
{


__int64 GetStreamOffset(IStream* pStream)
{
	ULARGE_INTEGER ulPos;
	LARGE_INTEGER li;
	li.QuadPart=0;
	pStream->Seek(li, STREAM_SEEK_CUR, &ulPos);

	return (__int64)ulPos.QuadPart;
}

__int64 GetStreamLength(IStream* pStream)
{
	__int64 iPos=GetStreamOffset(pStream);
	__int64 iEndPos=StreamSeek(pStream, 0, STREAM_SEEK_END);
	StreamSeek(pStream, iPos);
	return iEndPos;
}

__int64 StreamSeek(IStream* pStream, __int64 iOffset, DWORD dwOrigin)
{
	ULARGE_INTEGER ulPos;
	LARGE_INTEGER li;
	li.QuadPart=iOffset;
	pStream->Seek(li, dwOrigin, &ulPos);
	return (__int64)ulPos.QuadPart;
}

bool IsEOF(IStream* pStream)
{
	__int64 iPos=GetStreamOffset(pStream);
	__int64 iEndPos=StreamSeek(pStream, 0, STREAM_SEEK_END);
	StreamSeek(pStream, iPos);
	return iPos>=iEndPos;
}

HRESULT WriteStringToStream(IStream* pStream, const wchar_t* psz)
{
	// Work out length in bytes
	ULONG ulLen=(ULONG)(psz ? wcslen(psz) : 0);
	if (ulLen!=0)
	{
		ulLen=(ulLen+1) * sizeof(wchar_t);
	}

	// Write length
	ULONG cb;
	RETURNIFFAILED(pStream->Write((void*)&ulLen, sizeof(ulLen), &cb));

	// Write string data
	return ulLen ? pStream->Write((void*)psz, ulLen, &cb) : S_OK;
}


HRESULT ReadStringFromStream(IStream* pStream, CUniString& str)
{
	// Release old string
	str.Empty();

	// Read length
	ULONG ulLen=0;
	RETURNIFFAILED(MAPREADRESULT(pStream->Read(&ulLen, sizeof(ulLen), NULL)));

	// Quit if empty string
	if (!ulLen)
		return S_OK;

	// Read string data
	return pStream->Read(str.GetBuffer(ulLen/sizeof(wchar_t)-1), ulLen, NULL);
}


}	// namespace Simple
