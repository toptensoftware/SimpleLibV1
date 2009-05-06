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

unsigned int
gtou(
	giant 			x
)
/* Calculate the value of an int-sized unsigned giant NOT exceeding 32 bits. */
{
	register int 	size = abs(x->sign);

	switch(size)
	{
		case 0:
			break;
		case 1:
			return x->n[0];
		case 2:
			return x->n[0] | (x->n[1])<<16;
		default:
			fprintf(stderr,"Giant too large for gtou\n");
			break;
	}
	return 0;
}

void
utog(
	unsigned int	i,
	giant			g
)
/* The giant g becomes set to the integer value i. */
{
	if (i==0)
	{
		g->sign = 0;
		g->n[0] = 0;
		return;
	}
	g->n[0] = (unsigned short)(i & 0xFFFF);
	i >>= 16;
	if (i)
	{
		g->n[1] = (unsigned short)i;
		g->sign = 2;
	}
	else
	{
		g->sign = 1;
	}
}


void
bitandg(
	giant			a,
	giant			b
)
/* b := a & b, both a,b assumed non-negative. */
{
	int 			asize = abs(a->sign), bsize = abs(b->sign);
	int				commonsize=min(asize, bsize);
	int				j=0;
	unsigned short	*aptr = a->n, *bptr = b->n;


	for (j=0; j<commonsize; j++)
	{
		*bptr++ &= *aptr++;
	}

	// Truncate additional bits
	b->sign=commonsize;
}

void
bitorg(
	giant			a,
	giant			b
)
/* b := a & b, both a,b assumed non-negative. */
{
	int 			asize = abs(a->sign), bsize = abs(b->sign);
	int				commonsize=min(asize, bsize);
	int				j=0;
	unsigned short	*aptr = a->n, *bptr = b->n;


	for (j=0; j<commonsize; j++)
	{
		*bptr++ |= *aptr++;
	}
	for (; j<asize; j++)
	{
		*bptr++ = *aptr++;
	}
	b->sign=max(asize,bsize);
}

void
bitxorg(
	giant			a,
	giant			b
)
/* b := a ^ b, both a,b assumed non-negative. */
{
	int 			asize = abs(a->sign), bsize = abs(b->sign);
	int				commonsize=min(asize, bsize);
	int				j=0;
	unsigned short	*aptr = a->n, *bptr = b->n;


	for (j=0; j<commonsize; j++)
	{
		*bptr++ ^= *aptr++;
	}

	for (; j<asize; j++)
	{
		*bptr++ = *aptr++;
	}
	b->sign=max(asize,bsize);
}




}

