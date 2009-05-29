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

CUniString SIMPLEAPI UrlEncode(const wchar_t* p)
{
	CUniString str;
	while (p[0])
	{
		// Space?
		if (p[0]==' ')
		{
			str+=L"+";
			p++;
			continue;
		}

		// Non-ansi char?
		if (!iswalnum(p[0]))
		{
			str+=Format(L"%%%.2x", p[0]);
			p++;
			continue;
		}

		str.Append(p++, 1);
	}

	return str;
}

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

bool SIMPLEAPI DoWebPost(const wchar_t* pszUrl, const wchar_t* pszFormData, CUniString& str)
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

	// Check http:// and extract server name
	const wchar_t* p=StringSearch(pszUrl, L"http://", false);
	if (p!=pszUrl)
		return false;
	pszUrl+=7;

	// Find end of server
	p=StringSearch(p, L"/", false);
	if (!p)
		return false;

	// Connect to server
	CUniString strServer(pszUrl, int(p-pszUrl));
	HINTERNET hConnect=InternetConnect(hInternet, strServer, INTERNET_DEFAULT_HTTP_PORT,  NULL, NULL, INTERNET_SERVICE_HTTP, 0, 1);
	if (!hConnect)
	{
		InternetCloseHandle(hInternet);
		return false;
	}

	// Open HTTP request
	LPCWSTR ppszAccept[2]={L"*/*", NULL};
	HINTERNET hRequest=HttpOpenRequest(hConnect, L"POST", p, L"HTTP/1.0", NULL, ppszAccept, INTERNET_FLAG_RELOAD, 1);
	if (!hRequest)
	{
		InternetCloseHandle(hConnect);
		InternetCloseHandle(hInternet);
		return false;
	}

	// Send the request
	const wchar_t* pszHeaders=L"Content-type: application/x-www-form-urlencoded";
	CAnsiString strFormData=w2a(pszFormData);
	if (!HttpSendRequest(hRequest, pszHeaders, (DWORD)wcslen(pszHeaders), (void*)strFormData.sz(), (DWORD)strFormData.GetLength()))
	{
		InternetCloseHandle(hRequest);
		InternetCloseHandle(hConnect);
		InternetCloseHandle(hInternet);
		return false;
	}

	// Read it
	char sz[MAX_PATH+10];
	DWORD dwRead;
	while (InternetReadFile(hRequest, sz, MAX_PATH, &dwRead))
	{
		if (!dwRead)
			break;
		sz[dwRead]='\0';
		str.Append(a2w(sz));
	}

	InternetCloseHandle(hRequest);
	InternetCloseHandle(hConnect);
	InternetCloseHandle(hInternet);
	return true;
}




}