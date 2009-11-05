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

void SIMPLEAPI CalculateCRCStart(uint32_t& dwCRC);
void SIMPLEAPI CalculateCRCContinue(uint32_t& dwCRC, const void* pbData, int cbData);
void SIMPLEAPI CalculateCRCFinish(uint32_t& dwCRC);
uint32_t SIMPLEAPI CalculateCRC(const void* pbData, int cbData);

}	// namespace Simple

#endif	// __CALCCRC_H

