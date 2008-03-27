//////////////////////////////////////////////////////////////////////////
// GdiPlusLoader.h - declaration of GdiPlusLoader class

#ifndef __GDIPLUSLOADER_H
#define __GDIPLUSLOADER_H

namespace Simple
{


class CGdiPlusLoader
{
public:
	CGdiPlusLoader(bool bLoad=true);
	~CGdiPlusLoader();

	bool m_bLoaded;

	void DelayLoad();

	static void Load();
	static void Unload();
};



}	// namespace Simple

#endif	// __GDIPLUSLOADER_H

