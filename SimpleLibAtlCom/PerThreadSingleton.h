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
// AreFontsEqual.h - declaration of per-thread singleton implementation

#ifndef _PERTHREADSINGLETON_H
#define _PERTHREADSINGLETON_H


#ifdef __ATLCOM_H__

namespace Simple
{

// Singleton Class Factory
template <class T>
class CComClassFactorySingletonPerThread : public CComClassFactory
{
public:
	CComClassFactorySingletonPerThread()
	{
		m_pInstance=this;
	}

	virtual ~CComClassFactorySingletonPerThread()
	{
		m_pInstance=NULL;
	}

	void FinalRelease()
	{
	}

	T* FindSingletonInstance()
	{
		T* pInstFound=NULL;
		DWORD dwThisThreadID=GetCurrentThreadId();

		EnterCriticalSection(&_Module.m_csObjMap);

		// Check map...
		for (int i=0; i<m_Threads.GetSize(); i++)
			{
			if (m_Threads[i]->m_dwThreadID==dwThisThreadID)
				{
				pInstFound=m_Threads[i]->m_pObj;
				break;
				}
			}


		LeaveCriticalSection(&_Module.m_csObjMap);
		return pInstFound;
	}

	// IClassFactory
	STDMETHOD(CreateInstance)(LPUNKNOWN pUnkOuter, REFIID riid, void** ppvObj)
	{
		if (!ppvObj)
			return E_POINTER;

		// aggregation is not supported in Singletons
		ATLASSERT(pUnkOuter == NULL);
		if (pUnkOuter != NULL)
			return CLASS_E_NOAGGREGATION;

		HRESULT hr=E_FAIL;
		*ppvObj=NULL;

		DWORD dwThisThreadID=GetCurrentThreadId();

		EnterCriticalSection(&_Module.m_csObjMap);

		// Check map...
		for (int i=0; i<m_Threads.GetSize(); i++)
			{
			if (m_Threads[i]->m_dwThreadID==dwThisThreadID)
				{
				m_Threads[i]->m_pObj->GetUnknown()->QueryInterface(riid, ppvObj);
				hr=S_OK;
				break;
				}
			}

		// Not found?
		if (!*ppvObj)
			{
			T* p=new CComObject<T>();
			hr=p->FinalConstruct();
			if (FAILED(hr))
				{
				delete p;
				}
			else
				{
				hr=p->QueryInterface(riid, ppvObj);
				if (SUCCEEDED(hr))
					{
					m_Threads.InsertAt(0, new CThreadEntry(p));
					p->_singletonFactoryLink.Init(this);
					}
				else
					{
					delete p;
					}
				}
			}

		LeaveCriticalSection(&_Module.m_csObjMap);

		return hr;
	};

	void ThreadInstanceDestroyed()
	{
		EnterCriticalSection(&_Module.m_csObjMap);
		for (int i=0; i<m_Threads.GetSize(); i++)
			{
			if (m_Threads[i]->m_dwThreadID==GetCurrentThreadId())
				{
				m_Threads.DeleteAt(i);
				break;
				}
			}
		LeaveCriticalSection(&_Module.m_csObjMap);
	}

	class CThreadEntry
	{
	public:
		CThreadEntry(T* pObj) 
		{ 
			m_dwThreadID=GetCurrentThreadId(); 
			m_pObj=pObj; 
		};

		DWORD				m_dwThreadID;
		T*					m_pObj;
	};

	class CSingletonFactoryLink
	{
	public:
		CSingletonFactoryLink()
		{
			m_pFactory=NULL;
		};

		~CSingletonFactoryLink()
		{
			if (m_pFactory)
				m_pFactory->ThreadInstanceDestroyed();
		};

		void Init(CComClassFactorySingletonPerThread<T>* pFactory)
		{
			m_pFactory=pFactory;
		}

		CComClassFactorySingletonPerThread<T>*	m_pFactory;
	};
	

	CVector<CThreadEntry*>	m_Threads;
	static CComClassFactorySingletonPerThread<T>*	m_pInstance;
	static T* FindInstance()
	{
		if (!m_pInstance)
			return NULL;
			
		return m_pInstance->FindSingletonInstance();																	
	}
};

template <class T>
CComClassFactorySingletonPerThread<T>*	CComClassFactorySingletonPerThread<T>::m_pInstance=NULL;

#define DECLARE_CLASSFACTORY_SINGLETONPERTHREAD(obj) \
		DECLARE_CLASSFACTORY_EX(CComClassFactorySingletonPerThread<obj>) \
		CComClassFactorySingletonPerThread<obj>::CSingletonFactoryLink	_singletonFactoryLink; \
		static obj* GetInstance() { return CComClassFactorySingletonPerThread<obj>::FindInstance(); }


}	// namespace Simple

#endif		// __ATLCOM_H__

#endif		// _PERTHREADSINGLETON_H