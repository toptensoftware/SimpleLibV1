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
// SimpleDragDrop.h - declaration of CSimpleDragDrop class

#ifndef __SIMPLEDRAGDROP_H
#define __SIMPLEDRAGDROP_H

namespace Simple
{


class CSimpleDropSource :
	public IDropSource
{
public:
// Construction
			CSimpleDropSource();
	virtual ~CSimpleDropSource();


// IUnknown
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();
	STDMETHOD(QueryInterface)(REFIID iid, void ** ppvObject);

// IDropSource
	STDMETHODIMP QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState);
	STDMETHODIMP GiveFeedback(DWORD dwEffect);

	DWORD	m_dwButtonUsed;
	DWORD	m_dwCancelButton;
};

class CSimpleDropTarget : public IDropTarget
{
public:
// Construction
			CSimpleDropTarget();
	virtual ~CSimpleDropTarget();

// Operations
	HRESULT Register(HWND hWnd);
	HRESULT Revoke();

// IUnknown
	STDMETHOD_(ULONG, AddRef)();
	STDMETHOD_(ULONG, Release)();
	STDMETHOD(QueryInterface)(REFIID iid, void** ppvObject);

// IDropTarget
	STDMETHODIMP DragEnter(IDataObject* pDataObj, DWORD grfKeyState, POINTL ptIn, DWORD* pdwEffect)
	{
		POINT pt=SPoint(ptIn);
		ScreenToClient(m_hWndRegistered, &pt);
		return DragEnter(pDataObj, grfKeyState, pt, pdwEffect);
	}
	STDMETHODIMP DragOver(DWORD grfKeyState, POINTL ptIn, DWORD* pdwEffect)
	{
		POINT pt=SPoint(ptIn);
		ScreenToClient(m_hWndRegistered, &pt);
		return DragOver(grfKeyState, pt, pdwEffect);
	}

	STDMETHODIMP Drop(IDataObject* pDataObj, DWORD grfKeyState, POINTL ptIn, DWORD* pdwEffect)
	{
		POINT pt=SPoint(ptIn);
		ScreenToClient(m_hWndRegistered, &pt);
		return Drop(pDataObj, grfKeyState, pt, pdwEffect);
	}


	// Normalized overrides (POINT instead of POINTL + point already converted to client coords)
	virtual STDMETHODIMP DragEnter(IDataObject* pDataObj, DWORD grfKeyState, POINT pt, DWORD* pdwEffect)=0;
	virtual STDMETHODIMP DragOver(DWORD grfKeyState, POINT pt, DWORD* pdwEffect)=0;
	virtual STDMETHODIMP DragLeave()=0;
	virtual STDMETHODIMP Drop(IDataObject* pDataObj, DWORD grfKeyState, POINT pt, DWORD* pdwEffect)=0;

	// Attributes
	HWND		m_hWndRegistered;
};

}	// namespace Simple

#endif	// __SIMPLEDRAGDROP_H

