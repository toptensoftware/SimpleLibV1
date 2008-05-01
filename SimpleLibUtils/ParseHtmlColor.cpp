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
// ParseHtmlColor.cpp - implementation of ParseHtmlColor

#include "stdafx.h"
#include "SimpleLibUtilsBuild.h"

#include "ParseHtmlColor.h"


namespace Simple
{

struct COLORTABLEENTRY
{
	const wchar_t*	pszName;
	unsigned int ocValue;
};

COLORTABLEENTRY HtmlColors[] = 
{
	{ L"aliceblue", 0x00FFF8F0 }, 
	{ L"antiquewhite", 0x00D7EBFA }, 
	{ L"aqua", 0x00FFFF00 }, 
	{ L"aquamarine", 0x00D4FF7F }, 
	{ L"azure", 0x00FFFFF0 }, 
	{ L"beige", 0x00DCF5F5 }, 
	{ L"bisque", 0x00C4E4FF }, 
	{ L"black", 0x00000000 }, 
	{ L"blanchedalmond", 0x00CDEBFF }, 
	{ L"blue", 0x00FF0000 }, 
	{ L"blueviolet", 0x00E22B8A }, 
	{ L"brown", 0x002A2AA5 }, 
	{ L"burlywood", 0x0087B8DE }, 
	{ L"cadetblue", 0x00A09E5F }, 
	{ L"chartreuse", 0x0000FF7F }, 
	{ L"chocolate", 0x001E69D2 }, 
	{ L"coral", 0x00507FFF }, 
	{ L"cornflowerblue", 0x00ED9564 }, 
	{ L"cornsilk", 0x00DCF8FF }, 
	{ L"crimson", 0x003C14DC }, 
	{ L"cyan", 0x00FFFF00 }, 
	{ L"darkblue", 0x008B0000 }, 
	{ L"darkcyan", 0x008B8B00 }, 
	{ L"darkgoldenrod", 0x000B86B8 }, 
	{ L"darkgray", 0x00A9A9A9 }, 
	{ L"darkgreen", 0x00006400 }, 
	{ L"darkkhaki", 0x006BB7BD }, 
	{ L"darkmagenta", 0x008B008B }, 
	{ L"darkolivegreen", 0x002F6B55 }, 
	{ L"darkorange", 0x00008CFF }, 
	{ L"darkorchid", 0x00CC3299 }, 
	{ L"darkred", 0x0000008B }, 
	{ L"darksalmon", 0x007A96E9 }, 
	{ L"darkseagreen", 0x008BBC8F }, 
	{ L"darkslateblue", 0x008B3D48 }, 
	{ L"darkslategray", 0x004F4F2F }, 
	{ L"darkturquoise", 0x00D1CE00 }, 
	{ L"darkviolet", 0x00D30094 }, 
	{ L"deeppink", 0x009314FF }, 
	{ L"deepskyblue", 0x00FFBF00 }, 
	{ L"dimgray", 0x00696969 }, 
	{ L"dodgerblue", 0x00FF901E }, 
	{ L"firebrick", 0x002222B2 }, 
	{ L"floralwhite", 0x00F0FAFF }, 
	{ L"forestgreen", 0x00228B22 }, 
	{ L"fuchsia", 0x00FF00FF }, 
	{ L"gainsboro", 0x00DCDCDC }, 
	{ L"ghostwhite", 0x00FFF8F8 }, 
	{ L"gold", 0x0000D7FF }, 
	{ L"goldenrod", 0x0020A5DA }, 
	{ L"gray", 0x00808080 }, 
	{ L"green", 0x00008000 }, 
	{ L"greenyellow", 0x002FFFAD }, 
	{ L"honeydew", 0x00F0FFF0 }, 
	{ L"hotpink", 0x00B469FF }, 
	{ L"indianred", 0x005C5CCD }, 
	{ L"indigo", 0x0082004B }, 
	{ L"ivory", 0x00F0FFFF }, 
	{ L"khaki", 0x008CE6F0 }, 
	{ L"lavender", 0x00FAE6E6 }, 
	{ L"lavenderblush", 0x00F5F0FF }, 
	{ L"lawngreen", 0x0000FC7C }, 
	{ L"lemonchiffon", 0x00CDFAFF }, 
	{ L"lightblue", 0x00E6D8AD }, 
	{ L"lightcoral", 0x008080F0 }, 
	{ L"lightcyan", 0x00FFFFE0 }, 
	{ L"lightgoldenrodyellow", 0x00D2FAFA }, 
	{ L"lightgreen", 0x0090EE90 }, 
	{ L"lightgrey", 0x00D3D3D3 }, 
	{ L"lightpink", 0x00C1B6FF }, 
	{ L"lightsalmon", 0x007AA0FF }, 
	{ L"lightseagreen", 0x00AAB220 }, 
	{ L"lightskyblue", 0x00FACE87 }, 
	{ L"lightslategray", 0x00998877 }, 
	{ L"lightsteelblue", 0x00DEC4B0 }, 
	{ L"lightyellow", 0x00E0FFFF }, 
	{ L"lime", 0x0000FF00 }, 
	{ L"limegreen", 0x0032CD32 }, 
	{ L"linen", 0x00E6F0FA }, 
	{ L"magenta", 0x00FF00FF }, 
	{ L"maroon", 0x00000080 }, 
	{ L"mediumaquamarine", 0x00AACD66 }, 
	{ L"mediumblue", 0x00CD0000 }, 
	{ L"mediumorchid", 0x00D355BA }, 
	{ L"mediumpurple", 0x00DB7093 }, 
	{ L"mediumseagreen", 0x0071B33C }, 
	{ L"mediumslateblue", 0x00EE687B }, 
	{ L"mediumspringgreen", 0x009AFA00 }, 
	{ L"mediumturquoise", 0x00CCD148 }, 
	{ L"mediumvioletred", 0x008515C7 }, 
	{ L"midnightblue", 0x00701919 }, 
	{ L"mintcream", 0x00FAFFF5 }, 
	{ L"mistyrose", 0x00E1E4FF }, 
	{ L"moccasin", 0x00B5E4FF }, 
	{ L"navajowhite", 0x00ADDEFF }, 
	{ L"navy", 0x00800000 }, 
	{ L"oldlace", 0x00E6F5FD }, 
	{ L"olive", 0x00008080 }, 
	{ L"olivedrab", 0x00238E6B }, 
	{ L"orange", 0x0000A5FF }, 
	{ L"orangered", 0x000045FF }, 
	{ L"orchid", 0x00D670DA }, 
	{ L"palegoldenrod", 0x00AAE8EE }, 
	{ L"palegreen", 0x0098FB98 }, 
	{ L"paleturquoise", 0x00EEEEAF }, 
	{ L"palevioletred", 0x009370DB }, 
	{ L"papayawhip", 0x00D5EFFF }, 
	{ L"peachpuff", 0x00B9DAFF }, 
	{ L"peru", 0x003F85CD }, 
	{ L"pink", 0x00CBC0FF }, 
	{ L"plum", 0x00DDA0DD }, 
	{ L"powderblue", 0x00E6E0B0 }, 
	{ L"purple", 0x00800080 }, 
	{ L"red", 0x000000FF }, 
	{ L"rosybrown", 0x008F8FBC }, 
	{ L"royalblue", 0x00E16941 }, 
	{ L"saddlebrown", 0x0013458B }, 
	{ L"salmon", 0x007280FA }, 
	{ L"sandybrown", 0x0060A4F4 }, 
	{ L"seagreen", 0x00578B2E }, 
	{ L"seashell", 0x00EEF5FF }, 
	{ L"sienna", 0x002D52A0 }, 
	{ L"silver", 0x00C0C0C0 }, 
	{ L"skyblue", 0x00EBCE87 }, 
	{ L"slateblue", 0x00CD5A6A }, 
	{ L"slategray", 0x00908070 }, 
	{ L"snow", 0x00FAFAFF }, 
	{ L"springgreen", 0x007FFF00 }, 
	{ L"steelblue", 0x00B48246 }, 
	{ L"tan", 0x008CB4D2 }, 
	{ L"teal", 0x00808000 }, 
	{ L"thistle", 0x00D8BFD8 }, 
	{ L"tomato", 0x004763FF }, 
	{ L"turquoise", 0x00D0E040 }, 
	{ L"violet", 0x00EE82EE }, 
	{ L"wheat", 0x00B3DEF5 }, 
	{ L"white", 0x00FFFFFF }, 
	{ L"whitesmoke", 0x00F5F5F5 }, 
	{ L"yellow", 0x0000FFFF }, 
	{ L"yellowgreen", 0x0032CD9A },
	/*
	{ L"activeborder", 0x80000000 | COLOR_ACTIVEBORDER },
	{ L"activecaption", 0x80000000 | COLOR_ACTIVECAPTION },
	{ L"appworkspace", 0x80000000 | COLOR_APPWORKSPACE },
	{ L"background", 0x80000000 | COLOR_BACKGROUND },
	{ L"buttonface", 0x80000000 | COLOR_BTNFACE },
	{ L"buttonhighlight", 0x80000000 | COLOR_BTNHIGHLIGHT },
	{ L"buttonshadow", 0x80000000 | COLOR_BTNSHADOW },
	{ L"buttontext", 0x80000000 | COLOR_BTNTEXT },
	{ L"captiontext", 0x80000000 | COLOR_CAPTIONTEXT },
	{ L"graytext", 0x80000000 | COLOR_GRAYTEXT },
	{ L"highlight", 0x80000000 | COLOR_HIGHLIGHT },
	{ L"highlighttext", 0x80000000 | COLOR_HIGHLIGHTTEXT },
	{ L"inactiveborder", 0x80000000 | COLOR_INACTIVEBORDER },
	{ L"inactivecaption", 0x80000000 | COLOR_INACTIVECAPTION },
	{ L"inactivecaptiontext", 0x80000000 | COLOR_INACTIVECAPTIONTEXT },
	{ L"infobackground", 0x80000000 | COLOR_INFOBK },
	{ L"infotext", 0x80000000 | COLOR_INFOTEXT },
	{ L"menu", 0x80000000 | COLOR_MENU },
	{ L"menutext", 0x80000000 | COLOR_MENUTEXT },
	{ L"scrollbar", 0x80000000 | COLOR_SCROLLBAR },
	{ L"threeddarkshadow", 0x80000000 | COLOR_3DDKSHADOW },
	{ L"threedface", 0x80000000 | COLOR_3DFACE },
	{ L"threedhighlight", 0x80000000 | COLOR_3DHIGHLIGHT },
	{ L"threedlightshadow", 0x80000000 | COLOR_3DLIGHT },
	{ L"threedshadow", 0x80000000 | COLOR_3DSHADOW },
	{ L"window", 0x80000000 | COLOR_WINDOW },
	{ L"windowframe", 0x80000000 | COLOR_WINDOWFRAME },
	{ L"windowtext", 0x80000000 | COLOR_WINDOWTEXT },
	*/
};

bool FromHex(wchar_t ch, unsigned char* pVal)
{
	if (ch>=L'0' && ch<='9')
		{
		*pVal=ch-L'0';
		return true;
		}


	if (ch>=L'a' && ch<='f')
		{
		*pVal=ch-L'a'+10;
		return true;
		}

	if (ch>=L'A' && ch<='F')
		{
		*pVal=ch-L'A'+10;
		return true;
		}

	return false;
}

bool ReadHex(const wchar_t*& p, unsigned int* pdwVal)
{
	unsigned int dwVal=0;
	unsigned char b;
	if (!FromHex(p[0], &b))
		return false;
	p++;
	dwVal=b;

	while (FromHex(p[0], &b))
		{
		dwVal<<=4;
		dwVal|=b;
		p++;
		}
	*pdwVal=dwVal;
	return true;
}

bool SIMPLEAPI ParseHtmlColor(const wchar_t* psz, unsigned int* pVal)
{
	// RGB color value
	if (psz[0]==L'#')
		{
		psz++;
		unsigned int dwHex;
		if (!ReadHex(psz, &dwHex))
			return false;

		// Colors are encode BGR, not RGB
		*pVal=((dwHex & 0xFF)<<16) | 
				(dwHex & 0xFF00) | 
				((dwHex >> 16) & 0xFF);
		return true;
		}
	else
		{
		for (int i=0; i<_countof(HtmlColors); i++)
			{
			if (IsEqualStringI(psz, HtmlColors[i].pszName))
				{
				*pVal=HtmlColors[i].ocValue;
				return true;
				}
			}
		}
	return false;
}


} // namespace Simple

