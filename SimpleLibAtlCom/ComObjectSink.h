//////////////////////////////////////////////////////////////////////
//
// SimpleLib ATL COM Version 1.0
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
// ComObjectSink.h - declaration of CommandLineParser

#ifndef __COMOBJECTSINK_H
#define __COMOBJECTSINK_H

namespace Simple
{

// It is possible for Base not to derive from CComObjectRoot
// However, you will need to provide FinalConstruct and InternalQueryInterface
template <class Base>
class CComObjectSink : public Base
{
public:
	typedef Base _BaseClass;
	CComObjectSink(void* = NULL){m_hResFinalConstruct = FinalConstruct();}
	~CComObjectSink()
	{
		FinalRelease();
#ifdef _ATL_DEBUG_INTERFACES
		_Module.DeleteNonAddRefThunk(_GetRawUnknown());
#endif
	}

	STDMETHOD_(ULONG, AddRef)() {return 2;}
	STDMETHOD_(ULONG, Release)(){return 1;}
	STDMETHOD(QueryInterface)(REFIID iid, void ** ppvObject)
	{return _InternalQueryInterface(iid, ppvObject);}
	HRESULT m_hResFinalConstruct;
};

}	// namespace Simple

#endif		// __COMOBJECTSINK_H

