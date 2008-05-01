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
// SimpleLibWinMacros.h - declaration of AutoSizeControl

#ifndef __SIMPLELIBWINMACROS_H
#define __SIMPLELIBWINMACROS_H

namespace Simple
{

// Convert to variant bool
#ifndef BOOL2VBOOL
#define BOOL2VBOOL(b)  (VARIANT_BOOL)((b) ? VARIANT_TRUE : VARIANT_FALSE)
#endif

// Macro to calculate size of an array
#ifndef _countof
#define _countof(x) (sizeof(x)/sizeof(x[0]))
#endif

#ifndef RETURNIFFAILED
#define RETURNIFFAILED(x) { HRESULT hrx=(x); if (FAILED(hrx)) return hrx; }
#endif

#ifndef RETURNIFFALSE
#define RETURNIFFALSE(x) { HRESULT hrx=(x); if (hrx!=S_OK) return hrx; }
#endif

// Get a pointer to outer class of a nested class
#ifndef OUTERCLASS
#define OUTERCLASS(className, memberName) \
	reinterpret_cast<className*>(reinterpret_cast<LPBYTE>(this) - offsetof(className, memberName))
#endif


HINSTANCE GetModuleInstance();

}	// namespace Simple

#endif	// __SIMPLELIBWINMACROS_H

