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

#include "StdAfx.h"
#include "SimpleLibUtilsBuild.h"

#include "CalcCRC.h"

namespace Simple
{

static uint32_t g_dwCRCTable[256];
static bool  g_bInitialized=false;

// Reflection is a requirement for the official CRC-32 standard.
// You can create CRCs without it, but they won't conform to the standard.
static uint32_t CrcReflect(uint32_t ref, char ch)
{// Used only by _Init()

	uint32_t value(0);

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
	uint32_t ulPolynomial = 0x04c11db7;

	// 256 values representing ASCII character codes.
	for(int i = 0; i <= 0xFF; i++)
	{
		g_dwCRCTable[i]=CrcReflect(i, 8) << 24;
		for (int j = 0; j < 8; j++)
			g_dwCRCTable[i] = (g_dwCRCTable[i] << 1) ^ (g_dwCRCTable[i] & (1 << 31) ? ulPolynomial : 0);
		g_dwCRCTable[i] = CrcReflect(g_dwCRCTable[i], 32);
	}
}

void SIMPLEAPI CalculateCRCStart(uint32_t& dwCRC)
{
	// Make sure initialised
	InitCRCTable();
	dwCRC=0xFFFFFFFF;
}


void SIMPLEAPI CalculateCRCContinue(uint32_t& dwCRC, const void* pbDataIn, int cbData)
{
	const unsigned char* pbData=(const unsigned char*)pbDataIn;
	while(cbData--)
	{
		dwCRC = (dwCRC >> 8) ^ g_dwCRCTable[(dwCRC & 0xFF) ^ *pbData++];
	}
}

void SIMPLEAPI CalculateCRCFinish(uint32_t& dwCRC)
{
	// Exclusive OR the result with the beginning value.
	dwCRC = dwCRC ^ 0xffffffff;
}

uint32_t SIMPLEAPI CalculateCRC(const void* pbData, int cbData)
{
	uint32_t dwCRC;
	CalculateCRCStart(dwCRC);
	CalculateCRCContinue(dwCRC, pbData, cbData);
	CalculateCRCFinish(dwCRC);
	return dwCRC;
}




}	// namespace Simple
