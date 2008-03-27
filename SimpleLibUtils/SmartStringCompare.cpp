//////////////////////////////////////////////////////////////////////////
// SmartStringCompare.cpp - implementation of SmartStringCompare

#include "stdafx.h"
#include "SimpleLibUtilsBuild.h"

#include "SmartStringCompare.h"
#include "ParseUtils.h"

namespace Simple
{

int SmartStringCompare(const wchar_t* psz1, const wchar_t* psz2)
{
	while (psz1[0] && psz2[0])
		{
		if (iswdigit(psz1[0]) && iswdigit(psz2[0]))
			{
			int i1, i2;
			ReadInt(psz1, &i1);
			ReadInt(psz2, &i2);
			if (i1!=i2)
				return i1-i2;
			}
		else
			{
			int iCompare=_wcsnicmp(psz1, psz2, 1);
			if (iCompare!=0)
				return iCompare;
			psz1++;
			psz2++;
			}
		}

	if (psz1[0])
		return 1;
	if (psz2[0])
		return -1;
	return 0;
}





}	// namespace Simple
