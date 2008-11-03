//////////////////////////////////////////////////////////////////////
//
// SimpleLib Win Version 1.0
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
// ModuleCRC.cpp - implementation of ModuleCRC

#include "stdafx.h"
#include "SimpleLibWinBuild.h"
#include "StreamHelpers.h"

#include "ModuleCRC.h"

namespace Simple
{


CRCCONTROLBLOCK* LocateCRCControlBlock(LPBYTE p, unsigned int dwLen)
{
	while ((dwLen--)>sizeof(CRCCONTROLBLOCK))
	{
		// Cast to control block pointer
		CRCCONTROLBLOCK* pBlock=(CRCCONTROLBLOCK*)p;

		// Signatures match?
		if ((pBlock->dwCRCStartSig==CRC_SIG_START) && (pBlock->dwCRCEndSig==CRC_SIG_END))
			return reinterpret_cast<CRCCONTROLBLOCK*>(p);

		// Move on
		p++;
	}

	return NULL;
}

unsigned int CalculateModuleCRC(void* pModuleData, unsigned int dwLen, void* pCRCBlock)
{
	ASSERT(pCRCBlock!=NULL);
	ASSERT(reinterpret_cast<LPBYTE>(pCRCBlock)-reinterpret_cast<LPBYTE>(pModuleData)<(int)dwLen);

	unsigned int dwCRC=0;

	// Calculate CRC up to start of CRC control block
	CalculateCRCStart(dwCRC);
	CalculateCRCContinue(dwCRC, (LPBYTE)pModuleData, reinterpret_cast<LPBYTE>(pCRCBlock)-reinterpret_cast<LPBYTE>(pModuleData));

	// Calculate CRC after CRC control block
	CalculateCRCContinue(dwCRC, reinterpret_cast<LPBYTE>(pCRCBlock)+sizeof(CRCCONTROLBLOCK),
			dwLen-(reinterpret_cast<LPBYTE>(pCRCBlock)-reinterpret_cast<LPBYTE>(pModuleData))-sizeof(CRCCONTROLBLOCK));

	return dwCRC;
}

// Check module CRC
bool CheckModuleCRC(HINSTANCE hInstanceThis, bool* pbTamperFlag)
{
	static struct CRCCONTROLBLOCK ControlBloc =
		{ CRC_SIG_START, 0, 0, 0, CRC_SIG_END };

	// Get the module's file name
	WCHAR szModulePath[MAX_PATH];
	GetModuleFileName(hInstanceThis, szModulePath, MAX_PATH);

	// Load the file
	HANDLE hFile=CreateFile(szModulePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile)
	{
		// Get length of file
		unsigned int dwLen=SetFilePointer(hFile, 0, NULL, FILE_END);

		// Move to BOF
		SetFilePointer(hFile, 0, NULL, FILE_BEGIN);

		// Allocate buffer for text
		LPBYTE pModuleData=(LPBYTE)malloc(dwLen+1);

		// Read data
		DWORD dwBytesToRead=dwLen;
		if (ReadFile(hFile, pModuleData, dwLen, &dwBytesToRead, NULL))
		{
			// Locate control block
			CRCCONTROLBLOCK* pControlBlock=LocateCRCControlBlock(pModuleData, dwLen);

			if (pControlBlock && pControlBlock->dwModuleLength<=dwLen)
			{
				// Calculate CRC
				unsigned int dwCRC=CalculateModuleCRC(pModuleData, pControlBlock->dwModuleLength, pControlBlock);
				if (dwCRC==pControlBlock->dwCRC)
				{
					// Close file
					CloseHandle(hFile);

					// Release memory
					free(pModuleData);
					return true;
				}
			}
		}

		free(pModuleData);
	}

	return false;
}



#if 0

int CountCRCControlBlocks(LPBYTE p, DWORD dwLen)
{
	int iCount=0;
	while ((dwLen--)>sizeof(CRCCONTROLBLOCK))
	{
		// Cast to control block pointer
		CRCCONTROLBLOCK* pBlock=(CRCCONTROLBLOCK*)p;

		// Signatures match?
		if ((pBlock->dwCRCStartSig==CRC_SIG_START) &&

		(pBlock->dwCRCEndSig==CRC_SIG_END))
		iCount++;

		// Move on
		p++;
	}

	return iCount;
}

int main(int argc, char* argv[])
{
	// Open file for read/write
	FILE* pFile=fopen(argv[2], "rb");
	if (!pFile)
	{
		printf("Error: Can't open file '%s'\n", argv[2]);
		return 7;
	}

	// Get file size
	fseek(pFile, 0, SEEK_END);
	DWORD dwLength=ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	// Check has length
	if (dwLength==0)
	{
		fclose(pFile);
		printf("Error: Zero length file\n");
		return 7;
	}

	// Load file into memory
	LPBYTE pFileData=(LPBYTE)malloc(dwLength);

	// Read data
	if (fread(pFileData, dwLength, 1, pFile)!=1)
	{
		free(pFileData);
		fclose(pFile);
		printf("Error: Error reading file\n");
		return 7;
	}

	// Close the file
	fclose(pFile);

	if (CountCRCControlBlocks(pFileData, dwLength)>1)
		{
		free(pFileData);
		fclose(pFile);
		printf("Error: File contains multiple CRC Control blocks!\n");
		return 7;
		}
		

	// Locate CRC Control block
	CRCCONTROLBLOCK* pCRCBlock=LocateCRCControlBlock(pFileData, dwLength);
	if (!pCRCBlock)
	{
		free(pFileData);
		fclose(pFile);
		printf("Error: File doesn't contain CRC Control block!\n");
		return 7;
	}

	printf("CRC Control Block located at offset: 0x%.8x\n", reinterpret_cast<LPBYTE>(pCRCBlock)-pFileData);

	// Calculate the CRC upto start of control block
	pCRCBlock->dwCRC=CalculateModuleCRC(pFileData, dwLength, pCRCBlock);
	pCRCBlock->dwModuleLength=dwLength;
			
	char* pszTrunc=strstr(argv[2], ".notlocked");
	if (pszTrunc)
		pszTrunc[0]='\0';

	pFile=fopen(argv[2], "wb");
	if (!pFile)
	{
		printf("Error: Can't open file '%s'\n", argv[2]);
		return 7;
	}

	// Seek to CRC control block
	fwrite(pFileData, dwLength, 1, pFile);

	// Clean up
	free(pFileData);

	printf("Module CRC Locked - %s\n", argv[2]);

	// Done
	return 0;
}

#endif


}

