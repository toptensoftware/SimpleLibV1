//////////////////////////////////////////////////////////////////////
//
// SimpleLib ATL Win Version 1.0
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
// ResourceContentProvider.cpp - implementation of CBarResourceContentProviderImpl class

#include "stdafx.h"
#include "SimpleLibWinBuild.h"

#include "ResourceContentProvider.h"
#include "SimpleMemoryStream.h"
#include "LoadSaveTextStream.h"

namespace Simple
{

// Constructor
CResourceContentProvider::CResourceContentProvider()
{
	m_strResourceType=L"TEXT";
	m_pNext=NULL;
}

// Destructor
CResourceContentProvider::~CResourceContentProvider()
{
}

// Get file content from a resource...
bool CResourceContentProvider::GetFileContent(
			const wchar_t* pszCurrentFileName,
			const wchar_t* pszFileName, 
			bool bSysPath,
			CUniString& strQualifiedFileName,
			CUniString& strContent
			)
{
	// Scan all modules looking for matching resource...
	for (int i=0; i<m_vecModules.GetSize(); i++)
	{
		CAutoPtr<IStream, SRefCounted> spStream;
		if (SUCCEEDED(OpenResourceStream(m_vecModules[i], pszFileName, m_strResourceType, &spStream)))
		{
			strQualifiedFileName=Format(L"res://%s", pszFileName);
			return SUCCEEDED(LoadTextStream(spStream, strContent));		
		}
	}

	// Pass to next (if exists)
	if (m_pNext)
		return m_pNext->GetFileContent(pszCurrentFileName, pszFileName, bSysPath, strQualifiedFileName, strContent);

	// Not found
	return false;
}

void CResourceContentProvider::SetResourceType(const wchar_t* pszResourceType)
{
	m_strResourceType=pszResourceType;
}

void CResourceContentProvider::SetNextContentProvider(CContentProvider* pContentProvider)
{
	m_pNext=pContentProvider;
}

void CResourceContentProvider::AddModule(HMODULE hModule)
{
	m_vecModules.Add(hModule);
}


}	// namespace Simple
