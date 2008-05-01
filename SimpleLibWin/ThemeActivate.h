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
// ThemeActivate.h - declaration of ThemeActivate

#ifndef __THEMEACTIVATE_H
#define __THEMEACTIVATE_H

#ifndef SXS_MANIFEST_RESOURCE_ID
#define SXS_MANIFEST_RESOURCE_ID 2
#endif // SXS_MANIFEST_RESOURCE_ID

namespace Simple
{

// Public operations
void SIMPLEAPI InitThemeActivateImpl(HMODULE hMod, int id=SXS_MANIFEST_RESOURCE_ID);
void SIMPLEAPI CleanUpThemeActivateImpl();
void SIMPLEAPI ThemeActivate(HANDLE hContext, ULONG_PTR* pulCookie);
void SIMPLEAPI ThemeActivate(ULONG_PTR* pulCookie);
void SIMPLEAPI ThemeDeactivate(ULONG_PTR ulCookie);


#ifdef ISOLATION_AWARE_ENABLED

// Helper class to simplify matched calls to ThemeActivate/ThemeDeactivate
class CThemeActivate
{
public:
	CThemeActivate(bool bKill)	
	{
		m_ulCookie=NULL;
		if (bKill) 
			ThemeActivate(NULL, &m_ulCookie);
		else 
			ThemeActivate(&m_ulCookie); 
	};
	~CThemeActivate()	{ ThemeDeactivate(m_ulCookie); };
	ULONG_PTR	m_ulCookie;
};



#define InitThemeActivate		InitThemeActivateImpl
#define CleanUpThemeActivate	CleanUpThemeActivateImpl
#define THEME_ACTIVATE()	CThemeActivate	themeActivator(false);
#define THEME_DEACTIVATE()	CThemeActivate	themeActivator(true);

#else		// ISOLATION_AWARE_ENABLED

#define InitThemeActivate(hMod, id)
#define CleanUpThemeActivate()
#define THEME_ACTIVATE()			
#define THEME_DEACTIVATE()

#endif		// ISOLATION_AWARE_ENABLED


}	// namespace Simple

#endif	// __THEMEACTIVATE_H

