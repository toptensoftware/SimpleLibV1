//////////////////////////////////////////////////////////////////////////
// GdiText.h - declaration of GdiText

#ifndef __GDITEXT_H
#define __GDITEXT_H


namespace Simple
{

int SlxDrawText(HDC hdc, const wchar_t* pszString, int nCount, LPRECT lpRect, UINT uFormat);
SIZE MeasureDrawText(HDC hdc, HFONT hFont, const wchar_t* pszString, int iCount=-1, int iMaxWidth=0, UINT nFormat=DT_SINGLELINE);
SIZE MeasureText(HDC hdc, HFONT hFont, const wchar_t* pszString, int iCount=-1);
bool DrawTextIsCropped(HDC hDC, const wchar_t* psz, int cchText, LPRECT prc, UINT format);



}	// namespace Simple

#endif	// __GDITEXT_H


