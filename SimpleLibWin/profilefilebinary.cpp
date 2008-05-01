//////////////////////////////////////////////////////////////////////////
// ProfileFileBinary.cpp - implementation of ProfileFileBinary

#include "stdafx.h"
#include "SimpleLibWinBuild.h"

#include "ProfileFileBinary.h"
#include "StreamHelpers.h"

namespace Simple
{


#define SIGNATURE	('b' | ('i' << 8) | ('n' << 16) | ('i' << 24))
#define VERSION		1

struct BINARYHEADER
{
	DWORD	m_dwSignature;
	DWORD	m_dwVersion;
	DWORD	m_dwStringTableOffset;
	DWORD	m_dwStringTableCount;
	DWORD	m_dwDataOffset;
	DWORD	m_dwReserved[3];
};

struct CONTEXT
{
public:
	CStreamArchive ar;
	CIndex<CUniString, int>	StringMap;
	CIndex<int, CUniString> IDMap;
	CProfileFile*	m_pFile;
	int Allocate(const wchar_t* psz)
	{
		// Already in map?
		int iID;
		if (StringMap.Find(psz, iID))
			return iID;

		// No, allocate new ID
		iID=StringMap.GetSize();
		StringMap.Add(psz, iID);
		IDMap.Add(iID, psz);
		return iID;
	};
};

// Write a variable length dword 
static HRESULT WriteVarLen(IStream* pStream, DWORD dwVal, bool bCont=false)
{
	// Write MSB first
	if ((dwVal & 0xFFFFFF80)!=0)
		RETURNIFFAILED(WriteVarLen(pStream, dwVal>>7, true));

	BYTE b=(BYTE)(dwVal & 0x7F);
	if (bCont)
		b|=0x80;
	return pStream->Write(&b, sizeof(b), NULL);
}

static HRESULT WriteVarString(IStream* pStream, const CUniString& str)
{
	// Work out prefix
	DWORD dwPrefix=str.GetLength() << 1;

	// Scan if Unicode or ANSI
	const wchar_t* p=str;
	while (p[0])
	{
		if (HIBYTE(p[0]))
		{
			dwPrefix|=1;
			break;
		}
		p++;
	}

	// Write prefix...
	WriteVarLen(pStream, dwPrefix);

	if (str.GetLength()==0)
		return S_OK;

	if (dwPrefix & 1)
	{
		// Write as unicode
		ULONG cb;
		pStream->Write(static_cast<const wchar_t*>(str), str.GetLength() * sizeof(wchar_t), &cb);
	}
	else
	{
		// Write as ansi
		for (int i=0; i<str.GetLength(); i++)
		{
			ULONG cb;
			pStream->Write(&str[i], 1, &cb);
		}
	}

	return S_OK;
}

// Read a variable length dword from current position
static HRESULT ReadVarLen(IStream* pStream, DWORD& dwVal)
{
	// Read first byte
	BYTE c;
	RETURNIFFALSE(pStream->Read(&c, sizeof(c), NULL));

	dwVal=c;
	if (dwVal & 0x80)
		{
		dwVal &= 0x7f;
		do 
			{
			// Read next byte
			RETURNIFFALSE(pStream->Read(&c, sizeof(c), NULL));
	
			dwVal = (dwVal<<7) + (c & 0x7f);
			} while (c & 0x80);
		}

	return S_OK;
}

static HRESULT ReadVarString(IStream* pStream, CUniString& str)
{
	// Read prefix
	DWORD dwPrefix;
	RETURNIFFAILED(ReadVarLen(pStream, dwPrefix));

	int iLen=dwPrefix>>1;

	// Get buffer
	wchar_t* psz=str.GetBuffer(iLen);

	// Read characters
	if (dwPrefix & 1)
	{
		// Unicode
		ULONG cb;
		pStream->Read(psz, iLen * sizeof(wchar_t), &cb);
	}
	else
	{
		memset(psz, 0, sizeof(wchar_t) * iLen);

		// Ansi
		for (int i=0; i<iLen; i++)
		{
			ULONG cb;
			pStream->Read(psz+i, 1, &cb);
		}
	}

	return S_OK;
}


static void WriteEntry(CONTEXT& ctx, CProfileEntry* pEntry)
{
	WriteVarLen(ctx.ar, ctx.Allocate(pEntry->GetName()));
	WriteVarLen(ctx.ar, ctx.Allocate(pEntry->GetValue()));
}

static void WriteSection(CONTEXT& ctx, CProfileSection* pSection)
{
	// Write the section name
	WriteVarLen(ctx.ar, ctx.Allocate(pSection->GetName()));

	// Write the values
	WriteVarLen(ctx.ar, pSection->GetSize());
	for (int i=0; i<pSection->GetSize(); i++)
	{
		WriteEntry(ctx, pSection->GetAt(i));
	}

	// Write the sub-sections
	WriteVarLen(ctx.ar, pSection->GetSubSectionCount());
	for (int i=0; i<pSection->GetSubSectionCount(); i++)
	{
		WriteSection(ctx, pSection->GetSubSection(i));
	}
}

HRESULT SIMPLEAPI SaveBinaryProfile(const CProfileFile& Src, IStream* pStream)
{
	// Setup context
	CONTEXT ctx;
	ctx.ar=pStream;
	pStream->AddRef();

	// Setup header
	BINARYHEADER header;
	memset(&header, 0, sizeof(header));
	header.m_dwSignature=SIGNATURE;
	header.m_dwVersion=VERSION;

	ctx.ar << header;

	// Write all sections
	WriteVarLen(ctx.ar, Src.GetSize());
	for (int i=0; i<Src.GetSize(); i++)
	{
		WriteSection(ctx, Src[i]);
	}

	// Update header with string table info
	header.m_dwStringTableOffset=(DWORD)GetStreamOffset(pStream);
	header.m_dwStringTableCount=ctx.StringMap.GetSize();

	// Write string table
	for (int i=0; i<ctx.IDMap.GetSize(); i++)
	{
		WriteVarString(ctx.ar, ctx.IDMap[i].Value);
	}

	// Rewrite header
	StreamSeek(pStream, 0, STREAM_SEEK_SET);
	ctx.ar << header;

	// Done!
	return S_OK;
}

static HRESULT ReadSection(CONTEXT& ctx, CProfileSection* pParent)
{
	// Read name
	DWORD dwNameID;
	RETURNIFFAILED(ReadVarLen(ctx.ar, dwNameID));
	CUniString str=ctx.IDMap.Get(dwNameID, NULL);

	// Create the section
	CProfileSection* pSection;
	if (pParent)
		pSection=pParent->CreateSection(str);
	else
		pSection=ctx.m_pFile->CreateSection(str);

	// Read values
	int iNumValues;
	RETURNIFFAILED(ReadVarLen(ctx.ar, (DWORD&)iNumValues));
	for (int i=0; i<iNumValues; i++)
	{
		DWORD dwNameID;
		DWORD dwValueID;
		RETURNIFFAILED(ReadVarLen(ctx.ar, dwNameID));
		RETURNIFFAILED(ReadVarLen(ctx.ar, dwValueID));
		pSection->SetValue(ctx.IDMap.Get(dwNameID, NULL), ctx.IDMap.Get(dwValueID, NULL));
	}

	// Read sub-sections
	int iNumSubSections;
	RETURNIFFAILED(ReadVarLen(ctx.ar, (DWORD&)iNumSubSections));
	for (int i=0; i<iNumSubSections; i++)
	{
		RETURNIFFAILED(ReadSection(ctx, pSection));
	}

	return S_OK;
}

HRESULT SIMPLEAPI LoadBinaryProfile(CProfileFile& Dest, IStream* pStream)
{
	Dest.Reset(false);

	// Setup context
	CONTEXT ctx;
	ctx.ar=pStream;
	pStream->AddRef();
	ctx.m_pFile=&Dest;

	// Read header
	BINARYHEADER header;
	ctx.ar >> header;

	// Quit if wrong signature
	if (header.m_dwSignature!=SIGNATURE)
		return E_UNEXPECTED;

	// Read string table
	StreamSeek(ctx.ar, header.m_dwStringTableOffset);
	for (int i=0; i<int(header.m_dwStringTableCount); i++)
	{
		CUniString str;
		ReadVarString(ctx.ar, str);
		ctx.StringMap.Add(str,i);
		ctx.IDMap.Add(i, str);
	}

	// Seek back to start
	StreamSeek(pStream, sizeof(header));

	// Read sections...
	int iNumSubSections;
	RETURNIFFAILED(ReadVarLen(ctx.ar, (DWORD&)iNumSubSections));
	for (int i=0; i<iNumSubSections; i++)
	{
		RETURNIFFAILED(ReadSection(ctx, NULL));
	}

	ASSERT(GetStreamOffset(ctx.ar)==header.m_dwStringTableOffset);

	return S_OK;
}


}		// namespace Simple

