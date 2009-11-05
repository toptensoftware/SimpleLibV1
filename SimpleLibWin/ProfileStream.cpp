//////////////////////////////////////////////////////////////////////////
// ProfileStream.cpp - implementation of CProfileStream class

#include "stdafx.h"
#include "SimpleLibWinBuild.h"

#include "ProfileStream.h"
#include "StreamImpl.h"

namespace Simple
{

class CProfileStream :	public IStream
{
public:
	CProfileStream() 
	{
		m_pEntry=NULL;
		m_bDirty=false;
	}
	virtual ~CProfileStream() 
	{
		if (m_spMemStream)
			Commit(0);
	};

	HRESULT Create(CProfileEntry* pEntry)
	{
		if (!pEntry)
			return E_INVALIDARG;

		m_pEntry=pEntry;

		// Create memory stream
		RETURNIFFAILED(CreateStreamOnHGlobal(NULL, TRUE, &m_spMemStream));

		// Done
		return S_OK;
	}

	HRESULT Open(CProfileEntry* pEntry)
	{
		if (!pEntry)
			return E_INVALIDARG;

		// Unformat it
		HGLOBAL hData=NULL;
		unsigned char* pData;
		size_t cbData;
		if (UnformatBinaryData(pEntry->GetValue(), (void**)&pData, &cbData))
			{
			if (cbData)
				{
				hData=GlobalAlloc(GHND, cbData);
				memcpy(GlobalLock(hData), pData, cbData);
				GlobalUnlock(hData);
				}
			if (pData)
				free(pData);
			}

		// Store value name
		m_pEntry=pEntry;
		m_bDirty=false;

		// Create memory stream
		RETURNIFFAILED(CreateStreamOnHGlobal(hData, TRUE, &m_spMemStream));

		return S_OK;
	}

	bool				m_bDirty;
	CAutoPtr<IStream, SRefCounted>	m_spMemStream;
	CProfileEntry*		m_pEntry;

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

			// Get stream data
			HGLOBAL hData;
			GetHGlobalFromStream(m_spMemStream, &hData);

			// Convert to string
			CUniString strData;
			if (hData && stat.cbSize.QuadPart)
				{
				strData=FormatBinaryData((unsigned char*)GlobalLock(hData), (size_t)stat.cbSize.QuadPart, 64);
				GlobalUnlock(hData);
				}

			// Save it
			m_pEntry->SetValue(strData);
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

HRESULT SIMPLEAPI CreateProfileStream(CProfileEntry* pEntry, const wchar_t* pszSpec, IStream** pVal)
{
	if (!pEntry)
		return E_UNEXPECTED;

	if (!pszSpec)
		pszSpec=L"Streams\\Stream%.4i.bin";

	CProfileFile* pFile=pEntry->GetOwningFile();
	if (pFile && pFile->CreateOrOpenStream(pEntry, true, pszSpec, pVal))
	{
		return pVal[0] ? S_OK : E_FAIL;
	}

	// Create an instance of ProfileStream object
	CProfileStream* pStream=new CRefCountedStream<CProfileStream>();
	CAutoPtr<IStream, SRefCounted> spStream(pStream);

	// Initialise it
	RETURNIFFAILED(pStream->Create(pEntry));
	
	*pVal=spStream.Detach();
	return S_OK;
}

HRESULT SIMPLEAPI OpenProfileStream(CProfileEntry* pEntry, IStream** pVal)
{
	if (!pEntry)
		return E_UNEXPECTED;

	CProfileFile* pFile=pEntry->GetOwningFile();
	if (pFile && pFile->CreateOrOpenStream(pEntry, false, NULL, pVal))
	{
		return pVal[0] ? S_OK : E_FAIL;
	}

	// Create an instance of ProfileStream object
	CProfileStream* pStream=new CRefCountedStream<CProfileStream>();
	CAutoPtr<IStream, SRefCounted> spStream(pStream);

	// Initialise it
	RETURNIFFAILED(pStream->Open(pEntry));
	
	*pVal=spStream.Detach();
	return S_OK;
} 



}	// namespace Simple
