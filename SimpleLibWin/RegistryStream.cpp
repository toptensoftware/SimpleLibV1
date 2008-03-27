//////////////////////////////////////////////////////////////////////////
// RegStream.cpp - implementation of CRegStream class

#include "stdafx.h"
#include "SimpleLibWinBuild.h"

#include "RegistryStream.h"
#include "SmartHandle.h"

namespace Simple
{

class CRegStream : 
	public IStream
{
public:
	CRegStream() 
	{
		m_hKey=NULL;
		m_bDirty=false;
		m_dwRef=0;
	}
	virtual ~CRegStream() 
	{
		if (m_spMemStream)
			Commit(0);
	};


	HRESULT Create(HKEY hKey, const wchar_t* pszSubKey, const wchar_t* pszValueName)
	{
		m_bDirty=true;

		// Create sub key
		if (pszSubKey)
			{
			long l=RegCreateKey(hKey, pszSubKey, &m_hSubKey);
			if (l!=ERROR_SUCCESS)
				return HRESULT_FROM_WIN32(l);

			// Use sub key
			m_hKey=m_hSubKey;
			}
		else
			{
			m_hKey=hKey;
			}

		// Store value name
		m_strValueName=pszValueName;

		// Create memory stream
		RETURNIFFAILED(CreateStreamOnHGlobal(NULL, TRUE, &m_spMemStream));

		// Done
		return S_OK;
	}

	HRESULT Open(HKEY hKey, const wchar_t* pszSubKey, const wchar_t* pszValueName)
	{
		// Create sub key
		if (pszSubKey)
			{
			long l=RegOpenKeyEx(hKey, pszSubKey, 0, KEY_READ, &m_hSubKey);
			if (l!=ERROR_SUCCESS)
				return HRESULT_FROM_WIN32(l);

			// Use sub key
			m_hKey=m_hSubKey;
			}
		else
			{
			m_hKey=hKey;
			}

		// Get value type and size
		DWORD dwType, cbData=0;
		long lResult=RegQueryValueEx(m_hKey, pszValueName, NULL, &dwType, NULL, &cbData);
		if (lResult!=ERROR_SUCCESS)
			return HRESULT_FROM_WIN32(lResult);

		if (dwType!=REG_BINARY)
			return E_UNEXPECTED;

		// Read data
		cbData+=4;
		HGLOBAL hData=GlobalAlloc(GHND, cbData+4);
		lResult=RegQueryValueEx(m_hKey, pszValueName, NULL, &dwType, (LPBYTE)GlobalLock(hData), &cbData);
		GlobalUnlock(hData);
		if (lResult!=ERROR_SUCCESS)
			{
			GlobalFree(hData);
			return HRESULT_FROM_WIN32(lResult);
			}


		// Store value name
		m_strValueName=pszValueName;
		m_bDirty=false;

		// Create memory stream
		RETURNIFFAILED(CreateStreamOnHGlobal(hData, TRUE, &m_spMemStream));

		return S_OK;
	}

	bool				m_bDirty;
	CAutoPtr<IStream, SRefCounted>	m_spMemStream;
	CSmartHandle<HKEY>	m_hSubKey;
	HKEY				m_hKey;
	CUniString			m_strValueName;
	DWORD				m_dwRef;

	STDMETHODIMP_(ULONG) AddRef()
	{
		m_dwRef++;
		return m_dwRef;
	}
	STDMETHODIMP_(ULONG) Release()
	{
		m_dwRef--;
		if (m_dwRef)
			return m_dwRef;
		delete this;
		return 0;
	}
    STDMETHODIMP QueryInterface(REFIID riid, void **ppv)
	{
		if (riid==IID_IStream || riid==IID_ISequentialStream)
		{
			*ppv=(void*)static_cast<IStream*>(this);
			AddRef();
			return S_OK;
		}
		return E_NOINTERFACE;
	}



	STDMETHODIMP Read(void* pv, ULONG cb, ULONG* pcbRead)
	{
		return m_spMemStream->Read(pv, cb, pcbRead);
	};
	STDMETHODIMP Write(void const* pv, ULONG cb, ULONG* pcbWritten)
	{
		m_bDirty=true;
		return m_spMemStream->Write(pv, cb, pcbWritten);
	};
	STDMETHODIMP Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER* plibNewPosition)
	{
		return m_spMemStream->Seek(dlibMove, dwOrigin, plibNewPosition);
	};
	STDMETHODIMP SetSize(ULARGE_INTEGER libNewSize)
	{
		return m_spMemStream->SetSize(libNewSize);
	};
	STDMETHODIMP CopyTo(IStream* pstm, ULARGE_INTEGER cb, ULARGE_INTEGER* pcbRead, ULARGE_INTEGER* pcbWritten)
	{
		return m_spMemStream->CopyTo(pstm, cb, pcbRead, pcbWritten);
	};
	STDMETHODIMP Commit(DWORD grfCommitFlags)
	{
		RETURNIFFAILED(m_spMemStream->Commit(grfCommitFlags));

		if (m_bDirty)
			{
			m_bDirty=false;

			// Get stream size
			STATSTG stat;
			m_spMemStream->Stat(&stat, STATFLAG_NONAME);

			HGLOBAL hData;
			GetHGlobalFromStream(m_spMemStream, &hData);

			RegSetValueEx(m_hKey, m_strValueName, 0, REG_BINARY, (LPBYTE)GlobalLock(hData), (DWORD)stat.cbSize.QuadPart);
			GlobalUnlock(hData);
			}

		return S_OK;

	};
	STDMETHODIMP Revert()
	{
		return m_spMemStream->Revert();
	};
	STDMETHODIMP LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
	{
		return m_spMemStream->LockRegion(libOffset, cb, dwLockType);
	};
	STDMETHODIMP UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
	{
		return m_spMemStream->UnlockRegion(libOffset, cb, dwLockType);
	};
	STDMETHODIMP Stat(STATSTG* pstatstg, DWORD grfStatFlag)
	{
		return m_spMemStream->Stat(pstatstg, grfStatFlag);
	};
	STDMETHODIMP Clone(IStream** ppstm)
	{
		return E_NOTIMPL;
	};
};

HRESULT CreateRegistryStream(HKEY hKey, const wchar_t* pszSubKey, const wchar_t* pszValueName, IStream** pVal)
{
	// Create an instance of RegStream object
	CAutoPtr<CRegStream, SRefCounted> spRegStream=new CRegStream();

	// Initialise it
	RETURNIFFAILED(spRegStream->Create(hKey, pszSubKey, pszValueName));
	
	*pVal=spRegStream.Detach();
	return S_OK;
}

HRESULT OpenRegistryStream(HKEY hKey, const wchar_t* pszSubKey, const wchar_t* pszValueName, IStream** pVal)
{
	// Create an instance of RegStream object
	CAutoPtr<CRegStream, SRefCounted> spRegStream=new CRegStream();

	// Initialise it
	RETURNIFFAILED(spRegStream->Open(hKey, pszSubKey, pszValueName));
	
	*pVal=spRegStream.Detach();
	return S_OK;
} 



}	// namespace Simple
