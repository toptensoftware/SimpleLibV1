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
// DoCView.cpp - implementation of document/view class

#include "stdafx.h"
#include "SimpleLibWinBuild.h"

#include "DocView.h"
#include "MessageBox.h"
#include "SimpleFileDialog.h"
#include "FormatError.h"

namespace Simple
{
	namespace DocView
	{

//////////////////////////////////////////////////////////////////////////
// CView

// Constructor
CView::CView()
{
}

// Destructor
CView::~CView()
{
}


//////////////////////////////////////////////////////////////////////////
// CFrame

// Constructor
CFrame::CFrame()
{
}

// Destructor
CFrame::~CFrame()
{
}



//////////////////////////////////////////////////////////////////////////
// CDocument

// Constructor
CDocument::CDocument()
{
	m_strTitle=L"Untitled";
	m_bModified=false;
	m_bAutoDelete=false;
	CDocumentManager::GetInstance()->RegisterDocument(this);
}

// Destructor
CDocument::~CDocument()
{
	CDocumentManager::GetInstance()->RevokeDocument(this);
}

// Set the path name for this document
void CDocument::SetPathName(const wchar_t* pszPath)
{
	m_strPathName=pszPath;
	m_strTitle.Empty();
	if (!IsEmptyString(m_strPathName))
		{
		CUniString strTitle;
		SplitPath(m_strPathName, NULL, &strTitle);
		SetTitle(strTitle);
		}
	else
		{
		SetTitle(L"Untitled");
		}
}

// Get path name of this document
CUniString CDocument::GetPathName()
{
	return m_strPathName;
}

// Set title of this document
void CDocument::SetTitle(const wchar_t* pszTitle)
{
	m_strTitle=pszTitle;
	UpdateFrameTitles();
}

// Get title of this document
CUniString CDocument::GetTitle()
{
	return m_strTitle;
}

// Set/clear modified status 
void CDocument::SetModified(bool bModified)
{
	if (m_bModified!=bModified)
	{
		m_bModified=bModified;
		UpdateFrameTitles();
	}

}

// Get modified state
bool CDocument::GetModified()
{
	return m_bModified;
}

// Initialise new content for this document
HRESULT CDocument::OnInitNew()
{
	return S_OK;
}

// Save this document to a file
HRESULT CDocument::OnSaveDocument(const wchar_t* pszPath)
{	
	ASSERT(FALSE);
	return E_NOTIMPL;
}

// Load this document from file
HRESULT CDocument::OnLoadDocument(const wchar_t* pszPath)
{
	ASSERT(FALSE);
	return E_NOTIMPL;
}


// Reset content of this document
HRESULT CDocument::OnReset()
{
	return S_OK;
}

// Initialise new content
HRESULT CDocument::InitNew()
{
	// Delete old content, initialise new
	RETURNIFFAILED(OnReset());
	RETURNIFFAILED(OnInitNew());

	// Clear path and modified flag
	SetPathName(L"");
	SetTitle(L"Untitled");
	SetModified(false);

	UpdateAllViews(NULL, 0, 0);
	return S_OK;
}

// Save document to a file
HRESULT CDocument::Save(const wchar_t* pszPath)
{
	// Save it
	HRESULT hr=OnSaveDocument(pszPath);
	if (FAILED(hr))
		{
		// Delete file if save failed...
		DeleteFile(pszPath);

		if (FAILED(hr))
			SlxMessageBox(Format(L"%s\n\nFailed to save document - %s", pszPath, FormatError(hr)), MB_OK|MB_ICONERROR);

		return hr;
		}

	// Store path, clear modified
	SetPathName(pszPath);
	SetModified(false);

	return S_OK;
}

// Load document from a file
HRESULT CDocument::Load(const wchar_t* pszPath)
{
	// Clear path name
	m_strPathName.Empty();

	// Delete old content, load new
	RETURNIFFAILED(OnReset());
	HRESULT hr=OnLoadDocument(pszPath);
	if (hr!=S_OK)
		{
		OnReset();
		OnInitNew();

		SetModified(false);
		return hr;
		}

	// Store path (unless already set)
	if (IsEmptyString(m_strPathName))
	{
		SetPathName(pszPath);
	}

	// Clear modified flag
	SetModified(false);

	UpdateAllViews(NULL, 0, 0);

	return S_OK;
}

// Close document
HRESULT CDocument::Close()
{
	// Close all frames...
	CVector<CFrame*> AllFrames;
	int i;
	for (i=0; i<m_Views.GetSize(); i++)
	{
		CFrame* pFrame=m_Views[i]->GetFrame();
		if (AllFrames.Find(pFrame)<0)
			AllFrames.Add(pFrame);
	}

	// Close all frames...
	for (i=0; i<AllFrames.GetSize(); i++)
	{
		AllFrames[i]->OnDocumentClosing();
	}

	// Make sure all views are gone!
	ASSERT(m_Views.GetSize()==0);

	// Reset the document
	RETURNIFFAILED(OnReset());

	if (m_bAutoDelete)
		delete this;

	return S_OK;
}

bool CDocument::DoSaveDialog(bool bSaveAs)
{
	return false;
}

bool CDocument::IsMatchingDocument(const wchar_t* pszPath)
{
	return IsEqualStringI(GetPathName(), pszPath);
}


bool CDocument::PromptSaveChanges()
{
	if (!GetModified())
		return true;

	CUniString str=Format(L"Do you want to save changes to %s?", IsEmptyString(m_strPathName) ? m_strTitle : m_strPathName);

	switch (SlxMessageBox(str, MB_YESNOCANCEL|MB_ICONQUESTION))
		{
		case IDYES:
			return DoSaveDialog(false);

		case IDNO:
			return true;

		case IDCANCEL:
			return false;
		}

	return false;
}


void CDocument::AddView(CView* pView)
{
	m_Views.Add(pView);
	UpdateFrameTitles();
}

void CDocument::RemoveView(CView* pView)
{
	m_Views.Remove(pView);
	UpdateFrameTitles();
}

void CDocument::UpdateAllViews(CView* pSource, int iEvent, LPARAM lParam)
{
	for (int i=m_Views.GetSize()-1; i>=0; i--)
	{
		if (m_Views[i]!=pSource)
		{
			m_Views[i]->OnUpdate(pSource, iEvent, lParam);
		}
	}
}


void CDocument::SetAutoDelete(bool bAutoDelete)
{
	m_bAutoDelete=bAutoDelete;
}


CFrame* CDocument::GetFirstFrame()
{
	for (int i=0; i<m_Views.GetSize(); i++)
	{
		CFrame* pFrame=m_Views[i]->GetFrame();
		if (pFrame)
			return pFrame;
	}

	return NULL;
}

void CDocument::Activate()
{
	if (m_Views.GetSize()==0)
	{
		ASSERT(FALSE);
		return;
	}

	m_Views[0]->GetFrame()->Activate();
}

bool CDocument::CanCloseFrame(CFrame* pFrame)
{
	// Check if there are any other frames
	CVector<CFrame*> AllFrames;
	for (int i=0; i<m_Views.GetSize(); i++)
	{
		CFrame* pFrame=m_Views[i]->GetFrame();
		if (AllFrames.Find(pFrame)<0)
			AllFrames.Add(pFrame);
	}

	ASSERT(AllFrames.Find(pFrame)>=0);

	// If there are then frame can close...
	if (AllFrames.GetSize()>1)
		return true;

	// Only close if there are no other frames...
	if (!PromptSaveChanges())
		return false;

	// Close the document
	Close();

	// Return false as frame will already be destroyed
	return false;
}

void CDocument::UpdateFrameTitles()
{
	// Check if there are any other frames
	CVector<CFrame*> AllFrames;
	int i;
	for (i=0; i<m_Views.GetSize(); i++)
	{
		CFrame* pFrame=m_Views[i]->GetFrame();
		if (AllFrames.Find(pFrame)<0)
			AllFrames.Add(pFrame);
	}

	// Get current title
	CUniString strTitle=GetTitle();

	// Get modified flag
	bool bModified=GetModified();

	// Tell all frames
	for (i=0; i<AllFrames.GetSize(); i++)
	{
		AllFrames[i]->SetTitle(strTitle, AllFrames.GetSize()>1 ? i+1 : 0, bModified);
	}
}





/////////////////////////////////////////////////////////////////////////////
// CDocumentManager

// Constructor
CDocumentTemplate::CDocumentTemplate()
{
}

// Destructor
CDocumentTemplate::~CDocumentTemplate()
{
}

CUniString CDocumentTemplate::GetFilterTitle()
{
	return Format(L"%s Files (%s)", GetDisplayName(), GetFilterSpec());
}


// Check if able to open document	
int CDocumentTemplate::CanOpen(const wchar_t* pszPath)
{
	CVector<CUniString> vecFilters;
	SplitString(GetFilterSpec(), L";", vecFilters);

	CUniString strFileName;
	SplitPath(pszPath, NULL, &strFileName);

	for (int i=0; i<vecFilters.GetSize(); i++)
	{
		if (DoesWildcardMatch(vecFilters[i], strFileName))
			return 100;
	}

	return 0;
}

CUniString CDocumentTemplate::FormatFilter()
{
	return Format(L"%s|%s|All Files (*.*)|*.*||", GetFilterTitle(), GetFilterSpec());
}


/////////////////////////////////////////////////////////////////////////////
// CDocumentManager

// Constructor
CDocumentManager::CDocumentManager()
{
}

// Destructor
CDocumentManager::~CDocumentManager()
{
}


// Register a document template
void CDocumentManager::RegisterDocumentTemplate(CDocumentTemplate* pTemplate)
{
	m_Templates.Add(pTemplate);
}

// Revoke a document template
void CDocumentManager::RevokeDocumentTemplate(CDocumentTemplate* pTemplate)
{
	m_Templates.Remove(pTemplate);
}

// Register a new document
void CDocumentManager::RegisterDocument(CDocument* pDocument)
{
	m_Documents.Add(pDocument);
}

// Revoke a document
void CDocumentManager::RevokeDocument(CDocument* pDocument)
{
	m_Documents.Remove(pDocument);
}

// Open a document
HRESULT CDocumentManager::OpenDocument(const wchar_t* pszPathName, CDocument** pVal)
{
	// Check if already open
	if (pszPathName)
	{
		CDocument* pDocument=FindDocument(pszPathName);
		if (pDocument)
		{
			pDocument->Activate();
			*pVal=pDocument;
			return S_OK;
		}
	}

	// Find best template
	int iBest;
	CDocumentTemplate* pBest=NULL;
	for (int i=0; i<m_Templates.GetSize(); i++)
	{
		CDocumentTemplate* pTemplate=m_Templates[i];
		int iCanOpen=pTemplate->CanOpen(pszPathName);
		if (iCanOpen>0)
		{
			if (!pBest || iCanOpen>iBest)
			{
				pBest=pTemplate;
				iBest=iCanOpen;
			}
		}
	}

	// Quit if no suitable template
	if (!pBest)
		return E_UNEXPECTED;

	// Open the document...
	return pBest->OpenDocument(pszPathName, pVal);
}

CDocument* CDocumentManager::FindDocument(const wchar_t* pszPathName)
{
	if (!pszPathName)
		return NULL;

	// Look for matching document
	for (int i=0; i<m_Documents.GetSize(); i++)
	{
		if (m_Documents[i]->IsMatchingDocument(pszPathName))
			return m_Documents[i];		
	}

	return NULL;
}

bool CDocumentManager::CanClose()
{
	for (int i=0; i<m_Documents.GetSize(); i++)
	{
		if (!m_Documents[i]->PromptSaveChanges())
			return false;
	}

	return true;
}

bool CDocumentManager::CloseAllDocuments()
{
	for (int i=m_Documents.GetSize()-1; i>=0; i--)
	{
		m_Documents[i]->Close();
	}
	return true;
}


CUniString CDocumentManager::FormatFilter()
{
	if (m_Templates.GetSize()==0)
	{
		return L"All Files (*.*)|*.*||";
	}

	if (m_Templates.GetSize()==1)
	{
		return m_Templates[0]->FormatFilter();
	}

	CUniString buf;
	buf.Append(L"All Supported Files|");

	int i;
	for (i=0; i<m_Templates.GetSize(); i++)
	{
		if (i>0)
			buf.Append(L";");

		buf.Append(m_Templates[i]->GetFilterSpec());
	}

	for (i=0; i<m_Templates.GetSize(); i++)
	{
		buf.Append(L"|");
		buf.Append(m_Templates[i]->GetFilterTitle());
		buf.Append(L"|");
		buf.Append(m_Templates[i]->GetFilterSpec());
	}

	buf.Append(L"All Files (*.*)|*.*||");

	return buf;
}

int CDocumentManager::DoOpenFileDialog(HWND hWndParent, CUniString& str)
{
	CSimpleFileDialog dlg(false, FormatFilter(), L"");
	int iRetv=dlg.DoModal();
	if (iRetv!=IDOK)
		return iRetv;


	str=dlg.GetFilePath();
	return iRetv;
}

	} // namespace DocView

}	// namespace Simple
