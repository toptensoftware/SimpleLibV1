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
// FormatBinary.cpp - implementation of FormatBinary

#include "stdafx.h"
#include "SimpleLibUtilsBuild.h"

#include "FormatBinary.h"


namespace Simple
{



wchar_t SIMPLEAPI ToHex(int iNibble)
{
	return iNibble<10 ? (L'0' + iNibble) : (L'A' + iNibble -10);
}

unsigned char SIMPLEAPI FromHex(wchar_t ch)
{
	if (ch>=L'0' && ch<='9')
		return ch-L'0';

	if (ch>=L'a' && ch<='f')
		return ch-L'a'+10;

	if (ch>=L'A' && ch<='F')
		return ch-L'A'+10;

	ASSERT(false);
	return 0;
}

CUniString SIMPLEAPI FormatBinaryData(unsigned char* pData, size_t cb, int iBytesPerLine)
{
	// How many carriage returns 
	int iCRs=int(iBytesPerLine ? cb/iBytesPerLine : 0);

	// Allocate sys string
	CUniString str;
	wchar_t* p=str.GetBuffer(int(cb*2+1+iCRs*2));
	
	// Convert it
	for (size_t i=0; i<cb; i++)
		{
		p[0]=ToHex((*pData) >>4);
		p[1]=ToHex((*pData) & 0x0F);
		p+=2;

		if (iBytesPerLine && i>0 && (i % iBytesPerLine==0))
			{
			p[0]=L'\r';
			p[1]=L'\n';
			p+=2;
			}
		pData++;
		}
	p[0]=L'\0';

	return str;
}

bool SIMPLEAPI UnformatBinaryData(const wchar_t* psz, unsigned char** ppData, size_t* pcb)
{
	// How long is the string
	int iByteLen=int(wcslen(psz)/2);
	if (iByteLen<1)
		return false;

	*ppData=(unsigned char*)malloc(iByteLen);

	*pcb=UnformatBinaryData(psz, ppData[0], iByteLen);

	return true;
}

size_t SIMPLEAPI UnformatBinaryData(const wchar_t* psz, unsigned char* pData1, size_t cbData)
{
	// Write each byte to the stream
	const wchar_t* p=psz;
	unsigned char* pData=pData1;
	while (p[0])
		{
		unsigned char b;

		// Ignore whitespace
		while (p[0]==L' ' || p[0]==L'\r' || p[0]==L'\n')
			p++;

		if (!p[0])
			break;

		b=FromHex(p[0])<<4;
		p++;

		while (p[0]==L' ' || p[0]==L'\r' || p[0]==L'\n')
			p++;

		b |= FromHex(p[0]);
		p++;

		*pData++ = b;
		if ((size_t)(pData-pData1) > cbData)
			return cbData;
		}

	return pData-pData1;

}


}	// namespace Simple
