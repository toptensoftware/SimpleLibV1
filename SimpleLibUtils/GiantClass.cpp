//////////////////////////////////////////////////////////////////////
//
// SimpleLib Utils Version 1.0
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
// FormatBinary.h - declaration of FormatBinary

//////////////////////////////////////////////////////////////////////////
// Giant.cpp - implementation of CGiant

#include "stdafx.h"
#include "SimpleLibUtilsBuild.h"

#include "GiantClass.h"

namespace Simple
{

bool gparse(giant g, const wchar_t* psz, int iBase)
{
	while (psz[0])
	{
		smulg(iBase, g);

		int iDigit=psz[0];
		if (iDigit>='0' && iDigit<='9')
			iDigit-='0';
		else if (iDigit>='A' && iDigit<='Z')
			iDigit=iDigit-'A'+10;
		else if (iDigit>='a' && iDigit<='z')
			iDigit=iDigit-'a'+10;

		if (iDigit<0 || iDigit>=iBase)
		{
			return false;
		}

		iaddg(iDigit, g);

		psz++;
	}

	return true;
}

CUniString gformat(giant g, int iBase)
{
	if (isZero(g))
		return L"0";

	bool bNeg=gsign(g)==-1;

	CUniString str;
	while (!isZero(g))
	{
		int iDigit=idivg(iBase, g);
		if (iDigit<=9)
			str+=L'0'+iDigit;
		else
			str+=L'A'+(iDigit-10);
	}

	wchar_t* pA=str.GetBuffer();
	wchar_t* pB=pA+str.GetLength()-1;
	while (pA<pB)
	{
		Swap(*pA++, *pB--);
	}

	if (bNeg)
		return Format(L"-%s", str);
	else
		return str;
}

}

