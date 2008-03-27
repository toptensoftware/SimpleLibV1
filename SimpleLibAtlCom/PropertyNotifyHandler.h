//////////////////////////////////////////////////////////////////////////
// PropertyNotifyHandler.h - declaration of CPropertyNotifyHandler class

#ifndef __PROPERTYNOTIFYHANDLER_H
#define __PROPERTYNOTIFYHANDLER_H

#include "SimpleSink.h"

#ifndef _WIN64

namespace Simple
{

class CPNHDummy {};
typedef HRESULT(__stdcall CPNHDummy::*NOTIFYHANDLER)();

// CPropertyNotifyHandler Class
class CPropertyNotifyHandler : public CSimpleSink<IDispatch>
{
public:
// Construction
			CPropertyNotifyHandler();
	virtual ~CPropertyNotifyHandler();

// Attributes
	void Init(void* pInstance, NOTIFYHANDLER pfn);

// Operations

// Implementation
protected:
// Attributes
	void*			m_pInstance;
	union
		{
		NOTIFYHANDLER	m_pfn;
		DWORD			m_dwpfn;
		};

// Operations
	STDMETHODIMP GetTypeInfoCount(UINT* pctinfo);
	STDMETHODIMP GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo);
	STDMETHODIMP GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId);
	STDMETHODIMP Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr);
};


}	// namespace Simple

#endif		// !_WIN64

#endif	// __PROPERTYNOTIFYHANDLER_H

