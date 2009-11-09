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
// Result.h - declaration of Result

#ifndef __RESULT_H
#define __RESULT_H


namespace Simple
{

#ifdef _MSC_VER
typedef errno_t	error_t;
#endif

typedef uint32_t	result_t;

#define RESULT_TYPE(r)			((r) & 0xF0000000)
#define RESULT_TYPE_ERRNO		0x10000000
#define RESULT_TYPE_GENERIC		0x20000000

#define RESULT_CODE(r)			((r) & 0x0000FFFF)

inline result_t ResultFromErrno(error_t err)
	{ return err==0 ? 0 : (RESULT_TYPE_ERRNO | err); };

#define s_ok			0
#define e_fail			(RESULT_TYPE_GENERIC | 1)
#define e_notimpl		(RESULT_TYPE_GENERIC | 2)
#define e_invalidarg	(RESULT_TYPE_GENERIC | 3)
#define e_accessdenied	(RESULT_TYPE_GENERIC | 4)
#define e_outofmemory	(RESULT_TYPE_GENERIC | 5)
#define e_fileformat	(RESULT_TYPE_GENERIC | 6)
#define e_eof			(RESULT_TYPE_GENERIC | 7)

CUniString FormatResult(result_t r);

#define ReturnIfFailed(r) { result_t _rx=(r); if (_rx!=s_ok) return r; }

}



#endif	// __RESULT_H

