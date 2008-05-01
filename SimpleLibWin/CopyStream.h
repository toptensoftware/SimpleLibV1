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
// CopyStream.h - declaration of CopyStream

#ifndef __COPYSTREAM_H
#define __COPYSTREAM_H

namespace Simple
{

HRESULT SIMPLEAPI CopyStream(IStream* pTarget, IStream* pSource, DWORD dwMaxLength=0xFFFFFFFF);

}	// namespace Simple

#endif	// __COPYSTREAM_H

