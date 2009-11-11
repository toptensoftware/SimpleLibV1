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
// ModuleCRC.h - declaration of ModuleCRC

#ifndef __MODULECRC_H
#define __MODULECRC_H


namespace Simple
{

#define CODELOCK_TAMPERLOCK_INDEX		17

#define CRC_SIG_START	0x73737262
#define CRC_SIG_END		0x87259872

#pragma pack(1)
struct CRCCONTROLBLOCK
{
	unsigned int	dwCRCStartSig;
	unsigned int	dwCRC;
	unsigned int	dwModuleLength;
	unsigned int	dwReserved;
	unsigned int	dwCRCEndSig;
};
#pragma pack()

CRCCONTROLBLOCK* LocateCRCControlBlock(LPBYTE p, unsigned int dwLen);
unsigned int CalculateModuleCRC(void* pModuleData, unsigned int dwLen, void* pCRCBlock);
bool CheckModuleCRC(HINSTANCE hInstanceThis);
bool CheckModuleCRC(const wchar_t* pszModulePath);
bool CheckModuleCRC(void* pModuleData, unsigned int dwLen);


}	// namespace Simple

#endif	// __MODULECRC_H

