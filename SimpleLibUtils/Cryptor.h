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
// Cryptor.h - declaration of Cryptor

#ifndef __CRYPTOR_H
#define __CRYPTOR_H

namespace Simple
{

class CCryptorKey
{
public:
	CCryptorKey();
	CCryptorKey(const CCryptorKey& Other);
	CCryptorKey(unsigned int dwKey);
	CCryptorKey(unsigned int dwKey1, unsigned int dwKey2, unsigned int dwKey3);
	CCryptorKey(const char* pszKey);
	CCryptorKey(const wchar_t* pszKey);
	void SetStringKey(const char* pszKey);

	CCryptorKey& operator=(const CCryptorKey& Other);

// Operations
	unsigned char m_bKey[12];
};

class CCryptor
{
protected:
// Attributes
	unsigned long		m_LFSR_A;
	unsigned long		m_LFSR_B;
	unsigned long		m_LFSR_C;
	const unsigned long	m_Mask_A;
	const unsigned long	m_Mask_B;
	const unsigned long	m_Mask_C;
	const unsigned long	m_Rot0_A;
	const unsigned long	m_Rot0_B;
	const unsigned long	m_Rot0_C;
	const unsigned long	m_Rot1_A;
	const unsigned long	m_Rot1_B;
	const unsigned long	m_Rot1_C;
	unsigned long		m_LFSR_A_orig;
	unsigned long		m_LFSR_B_orig;
	unsigned long		m_LFSR_C_orig;
	uint64_t			m_uiPos;

public:
// Construction
			CCryptor();
	virtual ~CCryptor();

// Operations
	void SetKey(const CCryptorKey& Key);
	void TransformByte(unsigned char& Target);
	void TransformBuffer(void* pBuffer, unsigned int cbBuffer);
	void Skip(uint64_t cbBuffer);
	void Seek(uint64_t dwPos);
	uint64_t GetOffset() { return m_uiPos; }

	static void TransformBuffer(CCryptorKey& Key, void* pBuffer, unsigned int cbBuffer);
};

}	// namespace Simple


#endif	// __CRYPTOR_H

