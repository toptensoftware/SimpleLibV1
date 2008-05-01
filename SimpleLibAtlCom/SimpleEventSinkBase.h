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
// SimpleEventSinkBase.h - declaration of SimpleEventSinkBase

#ifndef __SIMPLEEVENTSINKBASE_H
#define __SIMPLEEVENTSINKBASE_H

#include "SimpleSink.h"

namespace Simple
{

HRESULT SIMPLEAPI GetObjectSourceInterface(IUnknown* pObject, IID* piidEvents, ITypeInfo** ppEventInfo);

// CSimpleEventSinkBase
class CSimpleEventSinkBase : public CSimpleSink<IDispatch>
{
public:
	CSimpleEventSinkBase()
	{
		m_pSource=NULL;
	};

	virtual ~CSimpleEventSinkBase()
	{
		Unadvise();
	}

	STDMETHOD(QueryInterface)(REFIID iid, void ** ppvObject)
	{
		if (iid==m_iidEvents)
			return CSimpleSink<IDispatch>::QueryInterface(IID_IDispatch, ppvObject);
		else
			return CSimpleSink<IDispatch>::QueryInterface(iid, ppvObject);
	}

	HRESULT PrepareTypeLib(IUnknown* pSource, REFIID riidEvents, ITypeInfo* pInfoEvents=NULL)
	{
		// Save event iid
		m_iidEvents=riidEvents;

		m_spEventInfo=pInfoEvents;
		if (!m_spEventInfo)
			{
			// If event info not specified, assume its defined in the same
			// type library as the disp interface of the object being advised on.


			// Need IDispatch for this to work
			CComQIPtr<IDispatch> spDisp(pSource);
			if (!spDisp)
				return E_NOINTERFACE;

			// Get the containing type libe
			CComPtr<ITypeInfo> spTypeInfo;
			RETURNIFFAILED(spDisp->GetTypeInfo(0, LOCALE_SYSTEM_DEFAULT, &spTypeInfo));
			CComPtr<ITypeLib> spTypeLib;
			RETURNIFFAILED(spTypeInfo->GetContainingTypeLib(&spTypeLib, NULL));

			// Look up the interface IID, usually it will be found!
			RETURNIFFAILED(spTypeLib->GetTypeInfoOfGuid(riidEvents, &m_spEventInfo));

			ASSERT(m_spEventInfo!=NULL);
			}

		return S_OK;
	}

	HRESULT PrepareTypeLib(IUnknown* pSource)
	{
		// Get info on source interface
		CComPtr<ITypeInfo> spEventInfo;
		IID iid;
		RETURNIFFAILED(GetObjectSourceInterface(pSource, &iid, &spEventInfo));

		return PrepareTypeLib(pSource, iid, spEventInfo);
	}

	HRESULT Advise(IUnknown* pSource, REFIID riidEvents, ITypeInfo* pInfoEvents=NULL, bool bStrongRef=true)
	{
		// Check not already connected
		ASSERT(!m_pSource);

		// Prepare type library
		PrepareTypeLib(pSource, riidEvents, pInfoEvents);

		// Connect
		RETURNIFFAILED(AtlAdvise(pSource, this, m_iidEvents, &m_dwAdvise));

		// Store source object
		m_pSource=pSource;
		m_bStrongRef=bStrongRef;
		if (m_bStrongRef)
			m_pSource->AddRef();

		// Done
		return S_OK;
	}

// Operations
	HRESULT Advise(IUnknown* pSource, bool bStrongRef=true)
	{
		// Check not already connected
		ASSERT(!m_pSource);

		PrepareTypeLib(pSource);

		// Connect
		RETURNIFFAILED(AtlAdvise(pSource, this, m_iidEvents, &m_dwAdvise));

		// Store source object
		m_pSource=pSource;
		m_bStrongRef=bStrongRef;
		if (m_bStrongRef)
			m_pSource->AddRef();

		// Done
		return S_OK;
	}

	HRESULT Unadvise()
	{
		if (m_pSource)
			{
			AtlUnadvise(m_pSource, m_iidEvents, m_dwAdvise);
			if (m_bStrongRef)
				m_pSource->Release();
			m_pSource=NULL;
			}
		m_spEventInfo.Release();
		return S_OK;
	}

public:
// Attributes
	IUnknown*			m_pSource;
	bool				m_bStrongRef;
	DWORD				m_dwAdvise;
	IID					m_iidEvents;
	CComPtr<ITypeInfo>	m_spEventInfo;

protected:
// IDispatch
	STDMETHODIMP GetTypeInfoCount(UINT* pctinfo)
	{
		return E_NOTIMPL;
	};
	STDMETHODIMP GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo)
	{
		return E_NOTIMPL;
	};
	STDMETHODIMP GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId)
	{
		if (m_spEventInfo)
			return m_spEventInfo->GetIDsOfNames(rgszNames, cNames, rgDispId);
		return E_NOTIMPL;
	};
	// Invoke not supplied
};


}	// namespace Simple

#endif	// __SIMPLEEVENTSINKBASE_H

