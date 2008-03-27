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
// CalcCRC.h - declaration of CalcCRC

#ifndef __CALCCRC_H
#define __CALCCRC_H

namespace Simple
{

void CalculateCRCStart(unsigned __int32& dwCRC);
void CalculateCRCContinue(unsigned __int32& dwCRC, const unsigned char* pbData, int cbData);
void CalculateCRCFinish(unsigned __int32& dwCRC);
unsigned __int32 CalculateCRC(const unsigned char* pbData, int cbData);

}	// namespace Simple

#endif	// __CALCCRC_H

