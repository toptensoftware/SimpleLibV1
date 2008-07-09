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
// CalcCRC.cpp - implementation of CalcCRC

#include "stdafx.h"
#include "SimpleLibUtilsBuild.h"

#include "CalcCRC.h"

namespace Simple
{


static unsigned __int32 g_dwCRCTable[256];
static bool  g_bInitialized=false;

// Another comment

// Reflection is a requirement for the official CRC-32 standard.
// You can create CRCs without it, but they won't conform to the standard.
static unsigned __int32 CrcReflect(unsigned __int32 ref, char ch)
{// Used only by _Init()

	unsigned __int32 value(0);

	// Swap bit 0 for bit 7
	// bit 1 for bit 6, etc.
	for(int i = 1; i < (ch + 1); i++)
	{
		if(ref & 1)
			value |= 1 << (ch - i);
		ref >>= 1;
	}
	return value;
}

static void InitCRCTable()
{
	// Quit if already initialised
	if (g_bInitialized)
		return;

	g_bInitialized = true;

	// This is the official polynomial used by CRC-32 
	// in PKZip, WinZip and Ethernet. 
	unsigned __int32 ulPolynomial = 0x04c11db7;

	// 256 values representing ASCII character codes.
	for(int i = 0; i <= 0xFF; i++)
	{
		g_dwCRCTable[i]=CrcReflect(i, 8) << 24;
		for (int j = 0; j < 8; j++)
			g_dwCRCTable[i] = (g_dwCRCTable[i] << 1) ^ (g_dwCRCTable[i] & (1 << 31) ? ulPolynomial : 0);
		g_dwCRCTable[i] = CrcReflect(g_dwCRCTable[i], 32);
	}
}

void SIMPLEAPI CalculateCRCStart(unsigned __int32& dwCRC)
{
	// Make sure initialised
	InitCRCTable();
	dwCRC=0xFFFFFFFF;
}


void SIMPLEAPI CalculateCRCContinue(unsigned __int32& dwCRC, const unsigned char* pbData, int cbData)
{
	while(cbData--)
		dwCRC = (dwCRC >> 8) ^ g_dwCRCTable[(dwCRC & 0xFF) ^ *pbData++];
}

void SIMPLEAPI CalculateCRCFinish(unsigned __int32& dwCRC)
{
	// Exclusive OR the result with the beginning value.
	dwCRC = dwCRC ^ 0xffffffff;
}

unsigned __int32 SIMPLEAPI CalculateCRC(const unsigned char* pbData, int cbData)
{
	unsigned __int32 dwCRC;
	CalculateCRCStart(dwCRC);
	CalculateCRCContinue(dwCRC, pbData, cbData);
	CalculateCRCFinish(dwCRC);
	return dwCRC;
}




}	// namespace Simple
