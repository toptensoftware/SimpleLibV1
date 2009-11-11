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
// ResBinary.cpp - implementation of CResNode class

#include "StdAfx.h"
#include "SimpleLibUtilsBuild.h"

#include "ResBinary.h"
#include "File.h"

namespace Simple
{

#define SIGNATURE	('b' | ('i' << 8) | ('n' << 16) | ('i' << 24))
#define VERSION		1

struct BINARYHEADER
{
	uint32_t	m_dwSignature;
	uint32_t	m_dwVersion;
	uint32_t	m_dwStringTableOffset;
	uint32_t	m_dwStringTableCount;
	uint32_t	m_dwDataOffset;
	uint32_t	m_dwReserved[3];
};

struct CONTEXT
{
public:
	IFile*					m_pFile;
	CIndex<CUniString, int>	StringMap;
	CIndex<int, CUniString>	IDMap;
	CResNode*				m_pNode;

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

// Write a variable length uint32_t
static result_t WriteVarLen(IFile* pFile, uint32_t dwVal, bool bCont=false)
{
	// Write MSB first
	if ((dwVal & 0xFFFFFF80)!=0)
		ReturnIfFailed(WriteVarLen(pFile, dwVal>>7, true));

	uint8_t b=(uint8_t)(dwVal & 0x7F);
	if (bCont)
		b|=0x80;
	return pFile->Write(&b, sizeof(b), NULL);
}

static result_t WriteVarString(IFile* pFile, const CUniString& str)
{
	// Work out prefix
	uint32_t dwPrefix=str.GetLength() << 1;

	// Scan if Unicode or ANSI
	const wchar_t* p=str;
	while (p[0])
	{
		if (p[0] & 0xFF00)
		{
			dwPrefix|=1;
			break;
		}
		p++;
	}

	// Write prefix...
	WriteVarLen(pFile, dwPrefix);

	if (str.GetLength()==0)
		return s_ok;

	if (dwPrefix & 1)
	{
		// Write as unicode
		uint32_t cb;
		pFile->Write(str.sz(), str.GetLength() * sizeof(wchar_t), &cb);
	}
	else
	{
		// Write as ansi
		for (int i=0; i<str.GetLength(); i++)
		{
			uint32_t cb;
			pFile->Write(&str[i], 1, &cb);
		}
	}

	return s_ok;
}

// Read a variable length uint32_t from current position
static result_t ReadVarLen(IFile* pFile, uint32_t& dwVal)
{
	// Read first byte
	uint8_t c;
	ReturnIfFailed(pFile->Read(&c, sizeof(c), NULL));

	dwVal=c;
	if (dwVal & 0x80)
		{
		dwVal &= 0x7f;
		do
			{
			// Read next byte
			ReturnIfFailed(pFile->Read(&c, sizeof(c), NULL));

			dwVal = (dwVal<<7) + (c & 0x7f);
			} while (c & 0x80);
		}

	return s_ok;
}

static result_t ReadVarString(IFile* pFile, CUniString& str)
{
	// Read prefix
	uint32_t dwPrefix;
	ReturnIfFailed(ReadVarLen(pFile, dwPrefix));

	int iLen=dwPrefix>>1;

	// Get buffer
	wchar_t* psz=str.GetBuffer(iLen);

	// Read characters
	if (dwPrefix & 1)
	{
		// Unicode
		ReturnIfFailed(pFile->Read(psz, iLen * sizeof(wchar_t), NULL));
	}
	else
	{
		memset(psz, 0, sizeof(wchar_t) * iLen);

		// Ansi
		for (int i=0; i<iLen; i++)
		{
			ReturnIfFailed(pFile->Read(psz+i, 1, NULL));
		}
	}

	return s_ok;
}


static void WriteNode(CONTEXT& ctx, CResNode* pNode)
{
	// Write the Node name
	WriteVarLen(ctx.m_pFile, ctx.Allocate(pNode->GetName()));

	// Write the values
	WriteVarLen(ctx.m_pFile, pNode->GetValueCount());
	for (int i=0; i<pNode->GetValueCount(); i++)
	{
		WriteVarLen(ctx.m_pFile, ctx.Allocate(pNode->GetValueName(i)));
		WriteVarLen(ctx.m_pFile, ctx.Allocate(pNode->GetValue(i)));
	}

	// Write the sub-Nodes
	WriteVarLen(ctx.m_pFile, pNode->GetNodeCount());
	for (int i=0; i<pNode->GetNodeCount(); i++)
	{
		WriteNode(ctx, pNode->GetNode(i));
	}
}

result_t SIMPLEAPI SaveBinaryRes(CResNode* pNode, IFile* pFile)
{
	// Setup context
	CONTEXT ctx;
	ctx.m_pFile=pFile;

	// Setup header
	BINARYHEADER header;
	memset(&header, 0, sizeof(header));
	header.m_dwSignature=SIGNATURE;
	header.m_dwVersion=VERSION;

	ReturnIfFailed(pFile->Write(&header, sizeof(header), NULL));

	// Write all Nodes
	WriteVarLen(pFile, pNode->GetNodeCount());
	for (int i=0; i<pNode->GetNodeCount(); i++)
	{
		WriteNode(ctx, pNode->GetNode(i));
	}

	// Update header with string table info
	uint64_t pos;
	ReturnIfFailed(pFile->GetCurrentPosition(&pos));
	header.m_dwStringTableOffset=(uint32_t)pos;
	header.m_dwStringTableCount=ctx.StringMap.GetSize();

	// Write string table
	for (int i=0; i<ctx.IDMap.GetSize(); i++)
	{
		WriteVarString(pFile, ctx.IDMap[i].Value);
	}

	// Rewrite header
	uint64_t i;
	ReturnIfFailed(pFile->Seek(0, OriginStart, &i));
	ReturnIfFailed(pFile->Write(&header, sizeof(header), NULL));

	// Done!
	return s_ok;
}

static result_t ReadNode(CONTEXT& ctx, CResNode* pParent)
{
	// Read name
	uint32_t dwNameID;
	ReturnIfFailed(ReadVarLen(ctx.m_pFile, dwNameID));
	CUniString str=ctx.IDMap.Get(dwNameID, NULL);

	// Create the Node
	CResNode* pNode=pParent->CreateNewNode(str);

	// Read values
	int iNumValues;
	ReturnIfFailed(ReadVarLen(ctx.m_pFile, (uint32_t&)iNumValues));
	for (int i=0; i<iNumValues; i++)
	{
		uint32_t dwNameID;
		uint32_t dwValueID;
		ReturnIfFailed(ReadVarLen(ctx.m_pFile, dwNameID));
		ReturnIfFailed(ReadVarLen(ctx.m_pFile, dwValueID));
		pNode->SetValue(ctx.IDMap.Get(dwNameID, NULL), ctx.IDMap.Get(dwValueID, NULL));
	}

	// Read sub-Nodes
	int iNumSubNodes;
	ReturnIfFailed(ReadVarLen(ctx.m_pFile, (uint32_t&)iNumSubNodes));
	for (int i=0; i<iNumSubNodes; i++)
	{
		ReturnIfFailed(ReadNode(ctx, pNode));
	}

	return s_ok;
}

result_t SIMPLEAPI LoadBinaryRes(CResNode* pDest, IFile* pFile)
{
	pDest->DeleteAllNodes();

	// Setup context
	CONTEXT ctx;
	ctx.m_pFile=pFile;
	ctx.m_pNode=pDest;

	// Read header
	BINARYHEADER header;
	ReturnIfFailed(ctx.m_pFile->Read(&header, sizeof(header), NULL));

	// Quit if wrong signature
	if (header.m_dwSignature!=SIGNATURE)
		return e_fileformat;

	// Read string table
	ctx.m_pFile->Seek(header.m_dwStringTableOffset);
	for (int i=0; i<int(header.m_dwStringTableCount); i++)
	{
		CUniString str;
		ReadVarString(ctx.m_pFile, str);
		ctx.StringMap.Add(str,i);
		ctx.IDMap.Add(i, str);
	}

	// Seek back to start
	pFile->Seek(sizeof(header));

	// Read Nodes...
	int iNumSubNodes;
	ReturnIfFailed(ReadVarLen(ctx.m_pFile, (uint32_t&)iNumSubNodes));
	for (int i=0; i<iNumSubNodes; i++)
	{
		ReturnIfFailed(ReadNode(ctx, pDest));
	}

	ASSERT(ctx.m_pFile->GetCurrentPosition()==header.m_dwStringTableOffset);

	return s_ok;
}

}
