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
// Cryptor.cpp - implementation of Cryptor

#include "stdafx.h"

#include "SimpleLibUtilsBuild.h"
#include "Cryptor.h"

namespace Simple
{
//////////////////////////////////////////////////////////////////////
// CCryptor implementation

// Constructor
CCryptorKey::CCryptorKey()
{
	memset(m_bKey, 0, sizeof(m_bKey));
}


// Constructor
CCryptorKey::CCryptorKey(const CCryptorKey& Other)
{
	memcpy(m_bKey, Other.m_bKey, 12);
}

// Constructor
CCryptorKey::CCryptorKey(unsigned int dwKey)
{
	unsigned int dwTriDWORD[3]=
		{dwKey, dwKey, dwKey};
	memcpy(m_bKey, dwTriDWORD, 12);
}

// Constructor
CCryptorKey::CCryptorKey(unsigned int dwKey1, unsigned int dwKey2, unsigned int dwKey3)
{
	unsigned int dwTriDWORD[3]=
		{dwKey1, dwKey2, dwKey3};
	memcpy(m_bKey, dwTriDWORD, 12);
}

// Constructor
CCryptorKey::CCryptorKey(const char* pszKey)
{
	SetStringKey(pszKey);
}

// Constructor
CCryptorKey::CCryptorKey(const wchar_t*pszKey)
{
	SetStringKey(w2a(pszKey));
}

CCryptorKey& CCryptorKey::operator=(const CCryptorKey& Other)
{
	memcpy(m_bKey, Other.m_bKey, 12);
	return *this;
}

// Constructor
void CCryptorKey::SetStringKey(const char* pszKey)
{
	// Set default key if none provided
	if ((pszKey==NULL) || (strlen(pszKey)==0))
		pszKey="DefSeedDefSeed";

	// Setup seed
	char szSeed[13];
	szSeed[12]='\0';
	strncpy_s(szSeed, sizeof(szSeed), pszKey, 12);

	// Make sure seed is at least 12 bytes long
	while (strlen(szSeed)<12)
	{
		strcat_s(szSeed, sizeof(szSeed), "Q");
	}

	memcpy(m_bKey, szSeed, 12);
}

// Constructor
CCryptor::CCryptor() :
		m_LFSR_A(0x13579BDF),
		m_LFSR_B(0x2468ACE0),
		m_LFSR_C(0xFDB97531),
		m_Mask_A(0x80000062),
		m_Mask_B(0x40000020),
		m_Mask_C(0x10000002),
		m_Rot0_A(0x7FFFFFFF),
		m_Rot0_B(0x3FFFFFFF),
		m_Rot0_C(0x0FFFFFFF),
		m_Rot1_A(0x80000000),
		m_Rot1_B(0xC0000000),
		m_Rot1_C(0xF0000000)
{
}

// Destructor
CCryptor::~CCryptor()
{
}

// Set key from a string of 12 bytes
void CCryptor::SetKey(const CCryptorKey& Key)
{
	int iIndex=0;

	// Setup LFSR registers
	for (iIndex=0; iIndex<4; iIndex++)
	{
		m_LFSR_A=((m_LFSR_A <<= 8) | ((unsigned long int) Key.m_bKey[iIndex+0]));
		m_LFSR_B=((m_LFSR_B <<= 8) | ((unsigned long int) Key.m_bKey[iIndex+4]));
		m_LFSR_C=((m_LFSR_C <<= 8) | ((unsigned long int) Key.m_bKey[iIndex+8]));
	}

	// Make sure nothing is zero
	if (m_LFSR_A==0x00000000)
		m_LFSR_A=0x13579BDF;
	if (m_LFSR_B==0x00000000)
		m_LFSR_B=0x2468ACE0;
	if (m_LFSR_C==0x00000000)
		m_LFSR_C=0xFDB97531;

	m_uiPos=0;
	m_LFSR_A_orig=m_LFSR_A;
	m_LFSR_B_orig=m_LFSR_B;
	m_LFSR_C_orig=m_LFSR_C;

	// Done
	return;
}

// Transform a single character
void CCryptor::TransformByte(unsigned char& Target)
{
	unsigned char Crypto=0;
	unsigned long int	Out_B = (m_LFSR_B & 0x00000001);
	unsigned long int	Out_C = (m_LFSR_C & 0x00000001);

	// Cycle the LFSR eight times to get eight pseudo random bits.
	for (int iCounter=0; iCounter<8; iCounter++)
	{
		if (m_LFSR_A & 0x00000001)
		{
			m_LFSR_A = ( (m_LFSR_A ^ (m_Mask_A >> 1)) | m_Rot1_A);

			if (m_LFSR_B & 0x00000001)
			{
				m_LFSR_B = ( (m_LFSR_B ^ (m_Mask_B >> 1)) | m_Rot1_B);
				Out_B=0x00000001;
			}
			else
			{
				m_LFSR_B = ( (m_LFSR_B >> 1) & m_Rot0_B);
				Out_B=0x00000000;
			}
		}
		else
		{
			m_LFSR_A = ( (m_LFSR_A >> 1) & m_Rot0_A);

			if (m_LFSR_C & 0x00000001)
			{
				m_LFSR_C = ( (m_LFSR_C ^ (m_Mask_C >> 1)) | m_Rot1_C);
				Out_C=0x00000001;
			}
			else
			{
				m_LFSR_C = ( (m_LFSR_C >> 1) & m_Rot0_C);
				Out_C=0x00000000;
			}
		}

		Crypto = (unsigned char)( (Crypto << 1) | (Out_B ^ Out_C));
	}

	// XOR resulting character with input character
	Target=(Target ^ Crypto);
	m_uiPos++;

	// Done
	return;
}

void CCryptor::TransformBuffer(void* pBuffer, unsigned int cbBuffer)
{
	for (unsigned int i=0; i<cbBuffer; i++)
	{
		TransformByte(reinterpret_cast<unsigned char*>(pBuffer)[i]);
	}
}

void CCryptor::Skip(unsigned __int64 cbBuffer)
{
	unsigned char b;
	for (unsigned __int64 i=0; i<cbBuffer; i++)
	{
		TransformByte(b);
	}
}

void CCryptor::Seek(unsigned __int64 uiPos)
{
	if (uiPos==m_uiPos)
		return;

	if (m_uiPos>uiPos)
	{
		// Back to start
		m_uiPos=0;
		m_LFSR_A=m_LFSR_A_orig;
		m_LFSR_B=m_LFSR_B_orig;
		m_LFSR_C=m_LFSR_C_orig;
	}

	// Skip forward
	Skip(uiPos-m_uiPos);
	ASSERT(m_uiPos==uiPos);
}

void CCryptor::TransformBuffer(CCryptorKey& Key, void* pBuffer, unsigned int cbBuffer)
{
	CCryptor Cryptor;
	Cryptor.SetKey(Key);
	Cryptor.TransformBuffer(pBuffer, cbBuffer);
}

}	// namespace Simple
