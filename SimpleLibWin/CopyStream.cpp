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
// CopyStream.cpp - implementation of CopyStream

#include "stdafx.h"
#include "SimpleLibWinBuild.h"

#include "CopyStream.h"

namespace Simple
{

// Copy everything from one stream to another
HRESULT SIMPLEAPI CopyStream(IStream* pTarget, IStream* pSource, DWORD dwMaxLength)
{
	BYTE bBuffer[1024];
	while (dwMaxLength)
		{
		// Read from source
		DWORD dwTryRead=min(dwMaxLength, sizeof(bBuffer));
		DWORD dwRead;
		RETURNIFFAILED(pSource->Read(bBuffer, dwTryRead, &dwRead));

		// Write to target
		if (dwRead)
			{
			RETURNIFFAILED(pTarget->Write(bBuffer, dwRead, NULL));
			dwMaxLength-=dwRead;
			}

		// End of stream?
		if (dwRead<sizeof(bBuffer))
			return S_OK;
		}

	return S_OK;
}


}	// namespace Simple
