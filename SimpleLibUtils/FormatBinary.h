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
// FormatBinary.h - declaration of FormatBinary

#ifndef __FORMATBINARY_H
#define __FORMATBINARY_H

namespace Simple
{

wchar_t SIMPLEAPI ToHex(int iNibble);
unsigned char SIMPLEAPI FromHex(wchar_t ch);
CUniString SIMPLEAPI FormatBinaryData(const void* pData, size_t cb, int iBytesPerLine);
bool SIMPLEAPI UnformatBinaryData(const wchar_t* psz, void** ppData, size_t* pcb);
size_t SIMPLEAPI UnformatBinaryData(const wchar_t* psz, void* pData, size_t cbData);

}	// namespace Simple

#endif	// __FORMATBINARY_H

