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
// MRUFileList.cpp - implementation of MRUFileList

#include "stdafx.h"
#include "SimpleLibWinBuild.h"

#include "MRUFileList.h"
#include "RegistryUtils.h"

namespace Simple
{

/////////////////////////////////////////////////////////////////////////////
// CMRUFileList

// Constructor
CMRUFileList::CMRUFileList()
{
	m_iMaxSize=4;
}

// Destructor
CMRUFileList::~CMRUFileList()
{
}

// Add a file (or bring it to the top)
void CMRUFileList::Add(LPCOLESTR pszPath)
{
	// Remove from list
	Remove(pszPath);

	// Insert at top
	InsertAt(0, pszPath);

	// Crop the list
	SetMaxSize(m_iMaxSize);
}

// Remove a file
void CMRUFileList::Remove(LPCOLESTR pszPath)
{
	int iPos=Find(pszPath);
	if (iPos>=0)
		RemoveAt(iPos);
}

// Set max size of list
void CMRUFileList::SetMaxSize(int iMax)
{
	m_iMaxSize=iMax;
	while (GetSize()>m_iMaxSize)
		{
		RemoveAt(GetSize()-1);
		}
}

// Save MRU file list to registry
void CMRUFileList::Save(LPCOLESTR pszSubKey, LPCOLESTR pszKey)
{
	// Work out full key
	CUniString strKey;
	if (!IsEmptyString(pszSubKey))
		strKey=SimplePathAppend(pszKey, pszSubKey);
	else
		strKey=pszKey;

	RegNukeKey(HKEY_CURRENT_USER, strKey);
	for (int i=0; i<GetSize(); i++)
	{
		RegSetString(HKEY_CURRENT_USER, strKey, Format(L"File%i", i), GetAt(i));
	}

	// Save count
	RegSetDWORD(HKEY_CURRENT_USER, strKey, L"MaxCount", m_iMaxSize);
}

// Load MRU file list from registry
void CMRUFileList::Load(LPCOLESTR pszSubKey, LPCOLESTR pszKey)
{
	// Remove old entries...
	RemoveAll();

	// Work out full key
	CUniString strKey;
	if (!IsEmptyString(pszSubKey))
		strKey=SimplePathAppend(pszKey, pszSubKey);
	else
		strKey=pszKey;

	CVector<CUniString> vecValues;
	RegEnumAllValues(HKEY_CURRENT_USER, strKey, vecValues);

	for (int i=0; i<vecValues.GetSize(); i++)
	{
		CUniString strFile;
		if (RegGetString(HKEY_CURRENT_USER, strKey, vecValues[i], strFile)==ERROR_SUCCESS)
		{
			_CVector::Add(strFile);
		}
	}

	DWORD dwCount=4;
	RegGetDWORD(HKEY_CURRENT_USER, strKey, L"MaxCount", (DWORD*)&dwCount);
	SetMaxSize(dwCount);
}

void CMRUFileList::Save(CProfileSection* pSection)
{
	pSection->RemoveAll();
	for (int i=0; i<GetSize(); i++)
	{
		pSection->SetValue(Format(L"File%i", i), GetAt(i));
	}
	pSection->SetIntValue(L"MaxCount", m_iMaxSize);
}

void CMRUFileList::Load(CProfileSection* pSection)
{
	RemoveAll();

	if (!pSection)
		return;

	for (int i=0; i<pSection->GetSize(); i++)
	{
		if (!IsEqualString(pSection->GetAt(i)->GetName(), L"MaxCount"))
		{
			_CVector::Add(pSection->GetAt(i)->GetValue());
		}
	}

	SetMaxSize(pSection->GetIntValue(L"MaxCount", 4));
}


bool FindMenuItem(HMENU hMenu, UINT nID, HMENU* phMenuFound, int* piPos)
{
	int iCount=GetMenuItemCount(hMenu);
	for (int i=0; i<iCount; i++)
		{
		if (GetMenuItemID(hMenu,i)==nID)
			{
			*phMenuFound=hMenu;
			*piPos=i;
			return true;
			}
		}

	return false;
}

// Update menu, removing old entries and inserting new ones...
void CMRUFileList::UpdateMenu(HMENU hMenu, UINT nID, LPCOLESTR pszEmpty, LPCOLESTR pszBaseDir, bool bTitleOnly)
{
	// Find menu position
	int iPos;
	if (!FindMenuItem(hMenu, nID, &hMenu, &iPos))
	{
		// Append to menu
		iPos=GetMenuItemCount(hMenu);
	}
	else
	{
		// Remove old entries...
		UINT nIDDel=nID;
		while (GetMenuItemID(hMenu, iPos)==nIDDel)
		{
			RemoveMenu(hMenu, iPos, MF_BYPOSITION);
			nIDDel++;
		}

		if (GetSubMenu(hMenu, iPos))
		{
			RemoveMenu(hMenu, iPos, MF_BYPOSITION);
		}
	}

	// Empty MRU?
	if (GetSize()==0)
	{
		InsertMenu(hMenu, iPos, MF_BYPOSITION|MF_STRING|MF_GRAYED, nID, pszEmpty ? pszEmpty : L"No Recent Files");
		return;
	}

	// If base directory not specified, use current directory
	OLECHAR szCurrentDir[MAX_PATH];
	if (!pszBaseDir)
	{
		GetCurrentDirectory(MAX_PATH, szCurrentDir);
		pszBaseDir=szCurrentDir;
	}

	// Update menu
	bool bSubMenu=false;
	for (int i=0; i<GetSize(); i++)
	{
		if (i>0 && (i % 10)==0)
		{
			HMENU hMoreMenu=CreatePopupMenu();
			InsertMenu(hMenu, iPos, MF_BYPOSITION|MF_POPUP, (UINT_PTR)hMoreMenu, L"&More");
			hMenu=hMoreMenu;
			iPos=0;
		}

		CUniString str;
		if (bTitleOnly)
			str=ExtractFileTitle(GetAt(i));
		else
			SplitPath(GetAt(i), NULL, &str);

		if (i+1>=10)
			str=Format(L"%i&%i %s", (i+1)/10, (i+1)%10, str);
		else
			str=Format(L"&%i %s", i+1, str);

		InsertMenu(hMenu, iPos++, MF_BYPOSITION|MF_STRING, nID+i, str);
	}

	// Done!
}


}