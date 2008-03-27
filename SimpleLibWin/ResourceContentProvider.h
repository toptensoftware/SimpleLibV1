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
// ResourceContentProvider.h

#ifndef __RESOURCECONTENTPROVIDER_H
#define __RESOURCECONTENTPROVIDER_H

namespace Simple
{

class CResourceContentProvider : public CContentProvider
{
public:
// Construction
			CResourceContentProvider();
	virtual ~CResourceContentProvider();

// CContentProvider
	virtual bool GetFileContent(
				const wchar_t* pszCurrentFileName,
				const wchar_t* pszFileName, 
				bool bSysPath,
				CUniString& strQualifiedFileName,
				CUniString& strContent
				);


// Operations
	void SetResourceType(const wchar_t* pszResourceType);
	void SetNextContentProvider(CContentProvider* pContentProvider);
	void AddModule(HMODULE hModule);

	CVector<HMODULE>	m_vecModules;
	CUniString			m_strResourceType;
	CContentProvider*	m_pNext;
};


}		// namespace Simple

#endif	// __RESOURCECONTENTPROVIDER_H

