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
// SimpleDragDrop.cpp - implementation of CSimpleDragDrop class

#include "stdafx.h"
#include "SimpleLibAtlComBuild.h"

#include "SimpleDragDrop.h"

namespace Simple
{

/////////////////////////////////////////////////////////////////////////////
// CSimpleDropSource

// Constructor
CSimpleDropSource::CSimpleDropSource()
{
	if (GetKeyState(VK_LBUTTON)<0)
	{
		m_dwButtonUsed=MK_LBUTTON;
		m_dwCancelButton=MK_RBUTTON;
	}
	else if (GetKeyState(VK_RBUTTON)<0)
	{
		m_dwButtonUsed=MK_RBUTTON;
		m_dwCancelButton=MK_LBUTTON;
	}

}

// Destructor
CSimpleDropSource::~CSimpleDropSource()
{
}

STDMETHODIMP_(ULONG) CSimpleDropSource::AddRef()
{
	return 2;
}

STDMETHODIMP_(ULONG) CSimpleDropSource::Release()
{
	return 1;
}

STDMETHODIMP CSimpleDropSource::QueryInterface(REFIID iid, void ** ppvObject)
{
	// Check params
	if (!ppvObject) 
		return E_POINTER;

	// IUnknown or IDispatch?
	if ((iid==IID_IUnknown) || (iid==IID_IDropSource))
		{
		AddRef();
		*ppvObject=static_cast<IDropSource*>(this);
		return S_OK;
		}

	// Unsupported interface
	return E_NOINTERFACE;
}


// Implementation of QueryContinueDrag
STDMETHODIMP CSimpleDropSource::QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState)
{
	// Cancel?
	if (fEscapePressed || (grfKeyState & m_dwCancelButton)!=0)
	{
		return DRAGDROP_S_CANCEL;
	}

	// Finished?
	if ((grfKeyState & m_dwButtonUsed)==0)
	{
		return DRAGDROP_S_DROP;
	}

	return S_OK;
}

// Implementation of GiveFeedback
STDMETHODIMP CSimpleDropSource::GiveFeedback(DWORD dwEffect)
{
	return DRAGDROP_S_USEDEFAULTCURSORS;
}



/////////////////////////////////////////////////////////////////////////////
// CSimpleDropTarget

// Constructor
CSimpleDropTarget::CSimpleDropTarget()
{
	m_hWndRegistered=NULL;
}

// Destructor
CSimpleDropTarget::~CSimpleDropTarget()
{
	if (m_hWndRegistered)
		Revoke();
}

HRESULT CSimpleDropTarget::Register(HWND hWnd)
{
	ASSERT(!m_hWndRegistered);

	RETURNIFFAILED(::RegisterDragDrop(hWnd, this));

	m_hWndRegistered=hWnd;
	return S_OK;
}

HRESULT CSimpleDropTarget::Revoke()
{
	if (m_hWndRegistered)
	{
		RevokeDragDrop(m_hWndRegistered);
		m_hWndRegistered=NULL;
	}

	return S_OK;
}

STDMETHODIMP_(ULONG) CSimpleDropTarget::AddRef()
{
	return 2;
}

STDMETHODIMP_(ULONG) CSimpleDropTarget::Release()
{
	return 1;
}

STDMETHODIMP CSimpleDropTarget::QueryInterface(REFIID iid, void ** ppvObject)
{
	// Check params
	if (!ppvObject) 
		return E_POINTER;

	// IUnknown or IDispatch?
	if ((iid==IID_IUnknown) || (iid==IID_IDropTarget))
		{
		AddRef();
		*ppvObject=static_cast<IDropTarget*>(this);
		return S_OK;
		}

	// Unsupported interface
	return E_NOINTERFACE;
}

}	// namespace Simple
