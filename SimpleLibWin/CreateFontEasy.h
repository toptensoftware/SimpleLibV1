//////////////////////////////////////////////////////////////////////////
// CreateFontEasy.h - declaration of CreateFontEasy

#ifndef __CREATEFONTEASY_H
#define __CREATEFONTEASY_H

namespace Simple
{

HFONT CreateFontEasy(const wchar_t* pszName, float Size, bool bBold=false, bool bItalic=false, bool bClearType=false);


}	// namespace Simple

#endif	// __CREATEFONTEASY_H

