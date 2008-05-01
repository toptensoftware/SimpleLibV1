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
// StreamHelpers.h - declaration of StreamHelpers

#ifndef __STREAMHELPERS_H
#define __STREAMHELPERS_H

namespace Simple
{

// Map S_FALSE to fail
inline HRESULT MapReadResult(HRESULT x)
{
	return x==S_FALSE ? E_FAIL : x;
};
#define MAPREADRESULT(x) MapReadResult(x)


// Seek helpers
__int64 SIMPLEAPI GetStreamOffset(IStream* pStream);
__int64 SIMPLEAPI GetStreamLength(IStream* pStream);
__int64 SIMPLEAPI StreamSeek(IStream* pStream, __int64 iOffset, DWORD dwOrigin=STREAM_SEEK_SET);
bool SIMPLEAPI IsEOF(IStream* pStream);

template <class T>
HRESULT SIMPLEAPI StreamRead(IStream* pStream, T& t)
{
	// Read
	ULONG cbRead;
	HRESULT hr=pStream->Read(&t, sizeof(t), &cbRead);

	// Quit if failed
	RETURNIFFAILED(hr);

	// Fail if didn't get everything 
	if (cbRead!=sizeof(t))
		return E_FAIL;

	// Done
	return hr;
}


template <class T>
HRESULT SIMPLEAPI StreamWrite(IStream* pStream, const T& t)
{
	// Read
	ULONG cbWrite;
	HRESULT hr=pStream->Write(&t, sizeof(t), &cbWrite);

	// Quit if failed
	RETURNIFFAILED(hr);

	// Fail if didn't write everything 
	if (cbWrite!=sizeof(t))
		return E_FAIL;

	// Done
	return hr;
}


// Write string to stream (compatible with ATL's BSTR to stream writing
HRESULT SIMPLEAPI WriteStringToStream(IStream* pStream, const wchar_t* psz);
HRESULT SIMPLEAPI ReadStringFromStream(IStream* pStream, CUniString& str);

class CStreamArchive : public CAutoPtr<IStream, SRefCounted>
{
public:
	CStreamArchive(IStream* pStream=NULL) : 
		CAutoPtr<IStream, SRefCounted>(pStream)
	{
		m_hr=S_OK;
	}

	template <class T>
	HRESULT operator << (const T& Value)
	{
		ASSERT(p!=NULL);
		RETURNIFFAILED(m_hr);
		return m_hr=StreamWrite(p, Value);
	}

	template <class T>
	HRESULT operator >> (T& Value)
	{
		ASSERT(p!=NULL);
		RETURNIFFAILED(m_hr);
		return m_hr=StreamRead(p, Value);
	}

	HRESULT operator << (const CUniString& Value)
	{
		ASSERT(p!=NULL);
		RETURNIFFAILED(m_hr);
		return m_hr=WriteStringToStream(p, Value);
	}

	HRESULT operator >> (CUniString& Value)
	{	
		ASSERT(p!=NULL);
		RETURNIFFAILED(m_hr);
		return m_hr=ReadStringFromStream(p, Value);
	}

#ifdef __ATLCOMCLI_H__
	HRESULT operator >> (ATL::CComBSTR& Value)
	{	
		ASSERT(p!=NULL);
		RETURNIFFAILED(m_hr);
		return m_hr=Value.ReadFromStream(p);
	}
	HRESULT operator << (const ATL::CComBSTR& Value)
	{	
		ASSERT(p!=NULL);
		RETURNIFFAILED(m_hr);
		return m_hr=const_cast<ATL::CComBSTR&>(Value).WriteToStream(p);
	}
#endif

	__int64 GetOffset()
	{
		ASSERT(p!=NULL);
		return GetStreamOffset(p);
	}
	__int64 Seek(__int64 iOffset, DWORD dwOrigin=STREAM_SEEK_SET)
	{
		return StreamSeek(p, iOffset, dwOrigin);
	}


	bool InError() { return FAILED(m_hr); };
	HRESULT GetResult() { return m_hr; };

protected:
	HRESULT m_hr;
};


}	// namespace Simple

#endif	// __STREAMHELPERS_H

