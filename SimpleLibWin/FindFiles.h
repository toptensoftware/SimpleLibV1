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
// FindFiles.h - declaration of FindFiles

#ifndef __FINDFILES_H
#define __FINDFILES_H

namespace Simple
{

void SIMPLEAPI FindFiles(const wchar_t* pszSpec, CVector<CUniString>& Files, bool bFullPath);
void SIMPLEAPI FindSubFolders(const wchar_t* pszFolder, CVector<CUniString>& Folders, bool bFullPath);

}	// namespace Simple

#endif	// __FINDFILES_H

