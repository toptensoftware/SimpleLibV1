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
// SimpleEventSink.h - declaration of SimpleEventSink

#ifndef __SIMPLEEVENTSINK_H
#define __SIMPLEEVENTSINK_H

#include "SimpleEventSinkBase.h"

namespace Simple
{

extern "C" HRESULT _invoke_c_this(
			void* pThis,
			int cArgs,
			VARIANT* pArgs,
			void* pfn
			);

extern "C" HRESULT _invoke_c(
			int cArgs,
			VARIANT* pArgs,
			void* pfn
			);


template <class T>
struct EVENTENTRY
{
	DISPID		m_iEventID;
	LPCOLESTR	m_pszEventName;
	union 
		{
		HRESULT (__stdcall T::*m_pfn)();	//method to invoke
		void* m_pvfn;
		};
};

// Helper functions for CSimpleEventSink
int CalcVariantStackSize(VARTYPE vt);
class CDummy {};
HRESULT SIMPLEAPI InvokeEventHandler(void* pThis, EVENTENTRY<CDummy>* pEventMap, ITypeInfo* pEventInfo, DISPID dispid, DISPPARAMS* pDispParams);



// CSimpleEventSink
template <class T>
class CSimpleEventSink : public CSimpleEventSinkBase
{
public:
	CSimpleEventSink()
	{
	};

	virtual ~CSimpleEventSink()
	{
	}

	STDMETHODIMP Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
	{
		ASSERT(m_spEventInfo!=NULL);
		T* pThis=static_cast<T*>(this);
		return InvokeEventHandler(pThis, 
					(EVENTENTRY<CDummy>*)pThis->GetEventMap(), 
					m_spEventInfo, dispIdMember, pDispParams);
	};

};


// Start of event map
#define BEGIN_SIMPLE_EVENT_MAP(c) \
	EVENTENTRY<c>* GetEventMap() {  \
	typedef c _event_classtype;\
	static EVENTENTRY<c> EventMap[] = {


#ifdef VC6
	// Event handler entry
	#define SIMPLE_EVENT_HANDLER(HandlerName) \
		{ 0, L#HandlerName, (HRESULT (__stdcall _event_classtype::*)())HandlerName },

	#define SIMPLE_EVENT_ID_HANDLER(id, HandlerName) \
		{ id, NULL, (HRESULT(__stdcall _event_classtype::*)())HandlerName },

	#define SIMPLE_EVENT_NAME_HANDLER(name, HandlerName) \
		{ 0, L##name, (HRESULT (__stdcall _event_classtype::*)())HandlerName },
#else
	// Event handler entry
	#define SIMPLE_EVENT_HANDLER(HandlerName) \
	{ 0, L#HandlerName, (HRESULT (__stdcall _event_classtype::*)())&_event_classtype::HandlerName },

	#define SIMPLE_EVENT_ID_HANDLER(id, HandlerName) \
	{ id, NULL, (HRESULT(__stdcall _event_classtype::*)())&_event_classtype::HandlerName },

	#define SIMPLE_EVENT_NAME_HANDLER(name, HandlerName) \
	{ 0, L##name, (HRESULT (__stdcall _event_classtype::*)())&_event_classtype::HandlerName },
#endif



// End of event map
#define END_SIMPLE_EVENT_MAP() \
	{ 0, NULL, NULL }, \
	}; \
	return EventMap; \
	};

}	// namespace Simple

#endif	// __SIMPLEEVENTSINK_H

