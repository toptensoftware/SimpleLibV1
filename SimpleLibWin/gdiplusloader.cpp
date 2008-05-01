//////////////////////////////////////////////////////////////////////////
// GdiPlusLoader.cpp - implementation of GdiPlusLoader class

#include "stdafx.h"
#include "SimpleLibWinBuild.h"

#include "GdiPlusLoader.h"

#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment(lib, "Gdiplus.lib")

namespace Simple
{

/////////////////////////////////////////////////////////////////////////////
// CGdiPlusLoader

#pragma warning(disable:4073)	// initializers put in library initialization area
#pragma init_seg(lib)
DWORD gdiplusRef=0;
GdiplusStartupInput gdiplusStartupInput;
ULONG_PTR gdiplusToken;

// Constructor
CGdiPlusLoader::CGdiPlusLoader(bool bLoad)
{
	m_bLoaded=bLoad;
	if (bLoad)
		Load();
}

// Destructor
CGdiPlusLoader::~CGdiPlusLoader()
{
	if (m_bLoaded)
		Unload();
}

void CGdiPlusLoader::DelayLoad()
{
	if (!m_bLoaded)
		{
		m_bLoaded=true;
		Load();
		}
}


void CGdiPlusLoader::Load()
{
	if (gdiplusRef==0)
		{
		GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
		}

	gdiplusRef++;
}

void CGdiPlusLoader::Unload()
{
	gdiplusRef--;
	if (gdiplusRef==0)
		{
		GdiplusShutdown(gdiplusToken);
		}
}



}	// namespace Simple
