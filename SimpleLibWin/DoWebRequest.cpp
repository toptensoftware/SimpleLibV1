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
// DoWebRequest.cpp - implementation of DoWebRequest

#include "stdafx.h"
#include "SimpleLibWinBuild.h"

#include "DoWebRequest.h"

namespace Simple
{

#include "wininet.h"
#pragma comment(lib, "wininet.lib")

bool SIMPLEAPI DoWebRequest(const wchar_t* pszUrl, CUniString& str)
{
	// Check connected
	DWORD dw;
	if (!InternetGetConnectedState(&dw, 0))
		return false;

	if ((dw & INTERNET_CONNECTION_MODEM)==0 && (dw & INTERNET_CONNECTION_OFFLINE))
		return false;

	// Open internet...
	HINTERNET hInternet=InternetOpen(L"ttsWebRequest", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, FALSE);
	if (!hInternet)
		return false;

	// Open URL
	HINTERNET hFile=InternetOpenUrl(hInternet, pszUrl, NULL, 0, 0, 
			INTERNET_FLAG_RELOAD|
			INTERNET_FLAG_NO_UI|
			INTERNET_FLAG_NO_AUTH|
			INTERNET_FLAG_NO_AUTO_REDIRECT|
			INTERNET_FLAG_PRAGMA_NOCACHE|
			INTERNET_FLAG_NO_CACHE_WRITE
			);
	if (!hFile)
		{
		InternetCloseHandle(hInternet);
		return false;
		}

	// Read it
	char sz[MAX_PATH+10];
	DWORD dwRead;
	while (InternetReadFile(hFile, sz, MAX_PATH, &dwRead))
		{
		if (!dwRead)
			break;
		sz[dwRead]='\0';
		str.Append(a2w(sz));
		}

	InternetCloseHandle(hFile);
	InternetCloseHandle(hInternet);

	return true;
}





}