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
// PerThreadData.h - declaration of PerThreadData 

#ifndef __PERTHREADDATA_H
#define __PERTHREADDATA_H

namespace Simple
{

extern DWORD g_dwPerThreadIndexNext;
extern DWORD g_dwPerThreadCount;
extern DWORD g_dwPerThreadTlsSlot;
extern class CPerThreadClassInfo* g_pFirstClassInfo;

typedef void (* PFNPERTHREADRELEASE)();

inline void ptdAllocTlsSlot()
{
	if (g_dwPerThreadTlsSlot==0)
		g_dwPerThreadTlsSlot=TlsAlloc();
}


class CPerThreadClassInfo
{
public:
	CPerThreadClassInfo(PFNPERTHREADRELEASE pfnRelease)
	{
		ptdAllocTlsSlot();

		m_pNext=g_pFirstClassInfo;
		g_pFirstClassInfo=this;
		m_pfnRelease=pfnRelease;

		if (m_dwIndex==0)
		{
			m_dwIndex=g_dwPerThreadIndexNext+1;
			g_dwPerThreadIndexNext++;
		}
	}

	PFNPERTHREADRELEASE			m_pfnRelease;
	CPerThreadClassInfo*		m_pNext;
	DWORD						m_dwIndex;
};

class CVectorLite
{
public:
	CVectorLite()
	{
		m_iSize=NULL;
		m_pData=NULL;
	}
	~CVectorLite()
	{
		if (m_pData)
			free(m_pData);
	}

	void GrowTo(int iSize)
	{
		if (iSize<=m_iSize)
			return;

		m_pData=(void**)realloc(m_pData, sizeof(void*) * iSize);
		memset(m_pData+m_iSize, 0, (iSize-m_iSize) * sizeof(void*));
		m_iSize=iSize;
	}
	int			m_iSize;
	void**		m_pData;
};

template<class T>
class CPerThreadData
{
public:

	static T* GetInstance()
	{
		// Make sure TLS allocated
		ptdAllocTlsSlot();

		// Make sure we have a thread index
		if (m_ClassInfo.m_dwIndex==0)
		{
			m_ClassInfo.m_dwIndex=g_dwPerThreadIndexNext+1;
			g_dwPerThreadIndexNext++;
		}

		// Get the per thread vector
		CVectorLite* pvec=(CVectorLite*)TlsGetValue(g_dwPerThreadTlsSlot);
		if (pvec==NULL)
		{
			pvec=new CVectorLite();
			TlsSetValue(g_dwPerThreadTlsSlot, pvec);
			pvec->GrowTo(g_dwPerThreadIndexNext);
		}
		else
		{
			// Make sure vector is big enough
			pvec->GrowTo(m_ClassInfo.m_dwIndex);
		}

		// Make sure object created
		if (!pvec->m_pData[m_ClassInfo.m_dwIndex-1])
		{
			pvec->m_pData[m_ClassInfo.m_dwIndex-1]=new T;
		}

		return (T*)pvec->m_pData[m_ClassInfo.m_dwIndex-1];
	}

	static void Release()
	{
		if (m_ClassInfo.m_dwIndex==0)
			return;

		CVectorLite* pvec=(CVectorLite*)TlsGetValue(g_dwPerThreadTlsSlot);
		if (pvec==NULL)
			return;

		if (pvec->m_pData[m_ClassInfo.m_dwIndex-1])
		{
			delete (T*)pvec->m_pData[m_ClassInfo.m_dwIndex-1];
			pvec->m_pData[m_ClassInfo.m_dwIndex-1]=NULL;
		}

		for (int i=0; i<pvec->m_iSize; i++)
		{
			if (pvec->m_pData[i]!=NULL)
				return;
		}

		delete pvec;

		TlsSetValue(g_dwPerThreadTlsSlot, NULL);
	}

	static CPerThreadClassInfo m_ClassInfo;
};

template<class T> CPerThreadClassInfo CPerThreadData<T>::m_ClassInfo(&CPerThreadData<T>::Release);


void ReleaseAllThreadData();

}

#endif	// __PERTHREADDATA_H

