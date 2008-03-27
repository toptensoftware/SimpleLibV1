//////////////////////////////////////////////////////////////////////
//
// SimpleLib ATL COM Version 1.0
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
// SimpleSink.h - declaration of CSimpleSink

#ifndef __SIMPLESINK_H
#define __SIMPLESINK_H

namespace Simple
{

template <class Itf, const IID* piid=&__uuidof(Itf)>
class CSimpleSink : public Itf
{
public:
	CSimpleSink() {}
	~CSimpleSink() {}

	STDMETHOD_(ULONG, AddRef)() { return 2; }
	STDMETHOD_(ULONG, Release)() { return 1; }
	STDMETHOD(QueryInterface)(REFIID iid, void ** ppvObject)
	{
		if (!ppvObject) return E_POINTER;
		if ((iid==IID_IUnknown) || (iid==*piid))
			{
			AddRef();
			*ppvObject=static_cast<Itf*>(this);
			return S_OK;
			}
		return E_NOINTERFACE;
	}
};

template <class Itf, const IID* piid=&__uuidof(Itf)>
class CSimpleSinkEx : public CSimpleSink<Itf, piid>
{
public:
	CSimpleSinkEx() 
	{
		m_dwCookie=0;
	}

	~CSimpleSinkEx() 
	{
		Unadvise();
	}

	HRESULT Advise(IUnknown* pObject)
	{
		ASSERT(!m_spSource);
		RETURNIFFAILED(AtlAdvise(pObject, this, *piid, &m_dwCookie));
		m_spSource=pObject;
		return S_OK;
	}

	HRESULT Unadvise()
	{
		if (m_spSource)
			{
			AtlUnadvise(m_spSource, *piid, m_dwCookie);
			m_spSource.Release();
			m_dwCookie=0;
			}
		return S_OK;
	}

	CComPtr<IUnknown>	m_spSource;
	DWORD	m_dwCookie;
};


}	// namespace Simple

#endif		// __SIMPLESINK_H

