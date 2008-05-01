//////////////////////////////////////////////////////////////////////////
// CreateFontEasy.cpp - implementation of CreateFontEasy

#include "stdafx.h"
#include "SimpleLibWinBuild.h"

#include "CreateFontEasy.h"


namespace Simple
{



HFONT SIMPLEAPI CreateFontEasy(const wchar_t* pszName, float Size, bool bBold, bool bItalic, bool bClearType)
{
	// Work out size
	HDC hDC=CreateIC(L"Display", NULL, NULL, NULL);
	POINT pt;
	pt.y = ::GetDeviceCaps(hDC, LOGPIXELSY) * int(Size * 100);
	pt.y /= 7200;    // 72 points/inch, 10 decipoints/point
	::DPtoLP(hDC, &pt, 1);
	POINT ptOrg = { 0, 0 };
	::DPtoLP(hDC, &ptOrg, 1);
	int iSize=-abs(pt.y - ptOrg.y);
	DeleteDC(hDC);

	LOGFONT lf;
	memset(&lf, 0, sizeof(lf));
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfHeight = iSize;
	lf.lfWeight=bBold ? FW_BOLD : FW_NORMAL;
	lf.lfItalic=bItalic ? 1 : 0;
	lf.lfQuality=bClearType ? 5 : DEFAULT_QUALITY;
	wcscpy_s(lf.lfFaceName, _countof(lf.lfFaceName), pszName);

	return CreateFontIndirect(&lf);
}



}	// namespace Simple
