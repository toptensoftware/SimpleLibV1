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
// LoadSaveTextStream.h - declaration of SaveTextStream

#ifndef __LOADSAVETEXTSTREAM_H
#define __LOADSAVETEXTSTREAM_H

#include "StreamHelpers.h"

namespace Simple
{



template <class TString>
HRESULT LoadTextStream(IStream* pStream, TString& str)
{
	int iLen=(int)GetStreamLength(pStream);

	// Read first two bytes
	wchar_t prefix;
	RETURNIFFAILED(MAPREADRESULT(pStream->Read(&prefix, sizeof(prefix), NULL)));

	if (prefix==0xFeFF)
	{
		CUniString buf;
		int iChars=iLen/sizeof(wchar_t)-1;
		RETURNIFFAILED(pStream->Read(buf.GetBuffer(iChars), iChars*sizeof(wchar_t), NULL));

		// Return Stream content
		str.Assign(buf);

		// Done
		return S_OK;
	}
	else
	{
		// Back to start
		StreamSeek(pStream, 0);

		// Read entire Stream in to ansi buffer
		CAnsiString buf;
		RETURNIFFAILED(MAPREADRESULT(pStream->Read(buf.GetBuffer(iLen), iLen, NULL)));

		// Return Stream content
		str.Assign(buf);

		// Done
		return S_OK;
	}
}

template <class TString>
HRESULT SaveTextStream(IStream* pStream, const TString* psz)
{
	// If unicode, write prefix character
	if (sizeof(TString)==sizeof(wchar_t))
	{
		wchar_t prefix=0xFEFF;
		RETURNIFFAILED(pStream->Write(&prefix, sizeof(prefix), NULL));
	}

	// Write the stirng
	return pStream->Write(psz, CString<TString>::len(psz) * sizeof(TString), NULL);
}

inline HRESULT SaveTextStream(IStream* pStream, const char* psz) 
	{ return SaveTextStream<char>(pStream, psz); };
inline HRESULT SaveTextStream(IStream* pStream, const wchar_t* psz) 
	{ return SaveTextStream<wchar_t>(pStream, psz); };

}	// namespace Simple

#endif	// __LOADSAVETEXTSTREAM_H

