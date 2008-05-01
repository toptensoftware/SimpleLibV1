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

/////////////////////////////////////////////////////////////////////////////
// SmartHandle.h - Handle wrappers

#ifndef _SMARTHANDLE_H
#define _SMARTHANDLE_H


namespace Simple
{

// GDI handle
class SGdiHandle
{
public:
	static void Release(HGDIOBJ hObj)
	{	
		DeleteObject(hObj);
	};
};

// HDC handle
class SDeleteDCHandle
{
public:
	static void Release(HDC hDC)
	{
		DeleteDC(hDC);
	}
};

// Win32 handle
class SWin32Handle
{
public:
	static void Release(HANDLE hObj)
	{	
		CloseHandle(hObj);
	};
};

// Menu handle
class SMenuHandle
{
public:
	static void Release(HMENU hObj)
	{
		DestroyMenu(hObj);
	}
};

// Icon handle
class SIconHandle
{
public:
	static void Release(HICON hIcon)
	{
		DestroyIcon(hIcon);
	}
};

// NB: Must use CSmartHandle<HGLOBAL, SGlobalHandle>
class SGlobalHandle
{
public:
	static void Release(HGLOBAL hGlobal)
	{
		GlobalFree(hGlobal);
	}
};

class SRegKey
{
public:
	static void Release(HKEY hKey)
	{
		RegCloseKey(hKey);
	}
};


// MSVC 6 Compiler gives internal compiler errors using
// assembler to access structure/class members offsets if anywhere
// in the project is a template class with no member variables.
// This fixes this.
#define MEMBERLESSCLASS \
	enum __FixCompilerInternalError;

// Automatic handle mapping
template <class T> 
class SAutoHandle 
{
	MEMBERLESSCLASS;
};
template <> class SAutoHandle<HBRUSH> : public SGdiHandle { };
template <> class SAutoHandle<HPEN> : public SGdiHandle { };
template <> class SAutoHandle<HFONT> : public SGdiHandle { };
template <> class SAutoHandle<HBITMAP> : public SGdiHandle { };
template <> class SAutoHandle<HRGN> : public SGdiHandle { };
template <> class SAutoHandle<HPALETTE> : public SGdiHandle { };
template <> class SAutoHandle<HANDLE> : public SWin32Handle { };
template <> class SAutoHandle<HMENU> : public SMenuHandle { };
template <> class SAutoHandle<HICON> : public SIconHandle { };
template <> class SAutoHandle<HDC> : public SDeleteDCHandle { };
template <> class SAutoHandle<HKEY> : public SRegKey { };

// Handle class
template <class T, class TSem=SAutoHandle<T> >
class CSmartHandle
{
public:
	CSmartHandle()
	{
		m_hObject=NULL;
	}
	CSmartHandle(T h)
	{
		m_hObject=h;
	}
	~CSmartHandle()
	{
		Release();
	}

	void Release()
	{
		if (m_hObject)
			TSem::Release(m_hObject);
		m_hObject=NULL;
	}

	T Detach()
	{
		T h=m_hObject;
		m_hObject=NULL;
		return h;
	}

	T operator=(T h)
	{
		Release();
		m_hObject=h;
		return h;
	}

	operator T()
	{
		return m_hObject;
	}

	T* operator&()
	{
		ASSERT(m_hObject==NULL);
		return &m_hObject;
	}

	T	m_hObject;
};


}	// namespace Simple

#endif		// _HANDLE_H
