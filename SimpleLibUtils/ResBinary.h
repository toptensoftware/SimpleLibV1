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
// ResBinary.h - declaration of CResNode class

#ifndef __RESBINARY_H
#define __RESBINARY_H

#include "ResNode.h"
#include "Result.h"


namespace Simple
{

struct IFile;

result_t SIMPLEAPI SaveBinaryRes(CResNode* pNode, IFile* pFile);
result_t SIMPLEAPI LoadBinaryRes(CResNode* pDest, IFile* pFile);

}

#endif	// __RESBINARY_H

