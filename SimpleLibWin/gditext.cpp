//////////////////////////////////////////////////////////////////////////
// GdiText.cpp - implementation of GdiText

#include "stdafx.h"
#include "SimpleLibWinBuild.h"

#include "GdiText.h"

#include "SelectObject.h"
#include "GeoOps.h"

namespace Simple
{


static int CountLines(const wchar_t* psz)
{
	int iCount=1;
	const wchar_t* p=psz;
	while (p[0])
		{
		if (p[0]==L'\r')
			{
			iCount++;
			if (p[1]=='\n')
				p++;
			}
		else if (p[0]==L'\n')
			{
			iCount++;
			}
		p++;
		}

	return iCount;
}

int SlxDrawText(HDC hdc, const wchar_t* pszString, int nCount, LPRECT lpRect, UINT uFormat)
{
	RECT rcTemp;
	if (uFormat & DT_VCENTER)
		{
		int iLines=CountLines(pszString);
		if (iLines>1)
			{
			TEXTMETRIC tm;
			GetTextMetrics(hdc, &tm);
			int iHeight=tm.tmHeight * iLines;

			rcTemp=*lpRect;
			rcTemp.top += (lpRect->bottom-lpRect->top-iHeight)/2;
			if (rcTemp.top < lpRect->top)
				rcTemp.top=lpRect->top;
			uFormat &= ~DT_VCENTER;
			uFormat &= ~DT_SINGLELINE;
			lpRect=&rcTemp;
			}
		}

	return DrawText(hdc, pszString, nCount, lpRect, uFormat);
}

SIZE MeasureDrawText(HDC hdc, HFONT hFont, const wchar_t* pszString, int iCount, int iMaxWidth, UINT nFormat)
{
	// Create a DC?
	HDC hDC=hdc;
	if (!hdc)
		{
		hDC=CreateIC(L"display", NULL, NULL, NULL);
		}

	// Select font
	CSelectObject hOldFont;
	if (hFont)
		{
		hOldFont.Select(hDC, hFont);
		}

	// Measure it
	RECT rc=SRect(0, 0, iMaxWidth, 0);
	DrawText(hDC, pszString, iCount, &rc, nFormat|DT_CALCRECT);

	// Restore font...
	if (hFont)
		hOldFont.Restore();

	// Clean up
	if (!hdc)
		{
		DeleteDC(hDC);
		}

	return Size(rc);
}


SIZE MeasureText(HDC hdc, HFONT hFont, const wchar_t* pszString, int iCount)
{
	// Create a DC?
	HDC hDC=hdc;
	if (!hdc)
		{
		hDC=CreateIC(L"display", NULL, NULL, NULL);
		}

	// Select font
	CSelectObject hOldFont;
	if (hFont)
		{
		hOldFont.Select(hDC, hFont);
		}

	if (iCount<0)
		iCount=int(wcslen(pszString));

	// Measure it
	SIZE size;
	GetTextExtentPoint32(hDC, pszString, iCount, &size);

	// Restore font...
	if (hFont)
		hOldFont.Restore();

	// Clean up
	if (!hdc)
		{
		DeleteDC(hDC);
		}

	return size;
}

bool DrawTextIsCropped(HDC hDC, const wchar_t* psz, int cchText, LPRECT prc, UINT format)
{
	CUniString str(psz);
	DrawText(hDC, str.GetBuffer(str.GetLength()+10), cchText, prc, format|DT_MODIFYSTRING);
	return !IsEqualString(str, psz);
}




}	// namespace Simple
