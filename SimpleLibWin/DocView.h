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
// DoCView.h - declaration of document/view class

#ifndef __DOCView_H
#define __DOCView_H

namespace Simple
{
	namespace DocView
	{

class CView;
class CFrame;
class CDocument;

class CView
{
public:
// Construction
			CView();
	virtual ~CView();

// Overrides
	virtual HRESULT Init(HWND hWndParent, CDocument* pDoc, CFrame* pFrame)=0;
	virtual CDocument* GetDocument()=0;
	virtual CFrame* GetFrame()=0;
	virtual void OnUpdate(CView* pSource, int iEvent, LPARAM lParam)=0;
	virtual HWND GetWindow()=0;
	virtual void OnActivate(bool bFrame) {};
	virtual void OnDeactivate(bool bFrame) {};
};

class CFrame
{
public:
// Construction
			CFrame();
	virtual ~CFrame();

// Update title of frame
	virtual void Activate()=0;
	virtual void SetTitle(const wchar_t* pszTitle, int iIndex, bool bModified)=0;
	virtual void OnDocumentClosing()=0;
	virtual void OnViewActivated(CView* pView) {};
	virtual void OnViewDestroyed(CView* pView) {};
	virtual void SetActiveView(CView* pView) {};
};

// CDocument Class
class CDocument
{
public:
// Construction
			CDocument();
	virtual ~CDocument();

// Attributes
	virtual void SetPathName(const wchar_t* pszPath);
	virtual CUniString GetPathName();
	virtual void SetTitle(const wchar_t* pszTitle);
	virtual CUniString GetTitle();
	virtual void SetModified(bool bModified=true);
	virtual bool GetModified();
	virtual HRESULT OnReset();
	virtual HRESULT OnInitNew();
	virtual HRESULT OnSaveDocument(const wchar_t* pszPath);
	virtual HRESULT OnLoadDocument(const wchar_t* pszPath);
	virtual bool PromptSaveChanges();
	virtual bool DoSaveDialog(bool bSaveAs);
	virtual bool IsMatchingDocument(const wchar_t* pszPath);
	virtual HRESULT CreateFrame() { return E_NOTIMPL; };
	virtual void Activate();
	virtual bool CanCloseFrame(CFrame* pFrame);
	virtual void UpdateFrameTitles();

// Operations
	HRESULT InitNew();
	HRESULT Save(const wchar_t* pszPath);
	HRESULT Load(const wchar_t* pszPath);
	HRESULT Close();

	void AddView(CView* pView);
	void RemoveView(CView* pView);
	void UpdateAllViews(CView* pSource, int iEvent, LPARAM lParam);
	
	CFrame* GetFirstFrame();
	void SetAutoDelete(bool bAutoDelete);

// Implementation
protected:
// Attributes
	CUniString				m_strPathName;
	CUniString				m_strTitle;
	bool					m_bModified;
	CVector<CView*>			m_Views;
	bool					m_bAutoDelete;


// Operations

};

class CDocumentTemplate
{
public:
// Construction
			CDocumentTemplate();
	virtual ~CDocumentTemplate();

// Operations
	virtual CUniString GetDisplayName()=0;
	virtual CUniString GetFilterSpec()=0;
	virtual CUniString GetFilterTitle();
	virtual int CanOpen(const wchar_t* pszPath);
	virtual HRESULT OpenDocument(const wchar_t* pszPath, CDocument** pVal)=0;

	CUniString FormatFilter();
};

template <class T>
class CDocumentTemplateImpl : public CDocumentTemplate
{
public:
	CDocumentTemplateImpl(const wchar_t* pszDisplayName, const wchar_t* pszFilterSpec) : 
		m_strDisplayName(pszDisplayName),
		m_strFilterSpec(pszFilterSpec)
	{
		ASSERT(CDocumentManager::GetInstance()!=NULL);
		CDocumentManager::GetInstance()->RegisterDocumentTemplate(this);
		m_iNextIndex=1;
	}
	virtual ~CDocumentTemplateImpl()
	{
		ASSERT(CDocumentManager::GetInstance()!=NULL);
		CDocumentManager::GetInstance()->RevokeDocumentTemplate(this);
	}
	virtual CUniString GetDisplayName()
	{
		return m_strDisplayName;
	}
	virtual CUniString GetFilterSpec()
	{
		return m_strFilterSpec;
	}
	virtual HRESULT OpenDocument(const wchar_t* pszPath, CDocument** pVal)
	{
		// Create new document
		T* pDocument=new T();

		// Load it
		if (pszPath)
		{
			HRESULT hr=pDocument->Load(pszPath);
			if (hr!=S_OK)
			{
				delete pDocument;
				return hr;
			}
		}
		else
		{
			HRESULT hr=pDocument->InitNew();
			pDocument->SetTitle(Format(L"Untitled %s %i", m_strDisplayName, m_iNextIndex++));
			if (hr!=S_OK)
			{
				delete pDocument;
				return hr;
			}
		}

		pDocument->SetAutoDelete(true);

		// Done
		*pVal=pDocument;
		return S_OK;
	}

// Attributes
	CUniString	m_strDisplayName;
	CUniString	m_strFilterSpec;
	int			m_iNextIndex;
};

// Document manager
class CDocumentManager : 
	public CSingleton<CDocumentManager>
{
public:
// Construction
			CDocumentManager();
	virtual ~CDocumentManager();

// Template operations
	void RegisterDocumentTemplate(CDocumentTemplate* pTemplate);
	void RevokeDocumentTemplate(CDocumentTemplate* pTemplate);

// Document operations
	void RegisterDocument(CDocument* pDocument);
	void RevokeDocument(CDocument* pDocument);
	virtual HRESULT OpenDocument(const wchar_t* pszPathName, CDocument** pVal);
	virtual CDocument* FindDocument(const wchar_t* pszPathName);

	bool CanClose();
	bool CloseAllDocuments();

	CUniString FormatFilter();
	int DoOpenFileDialog(HWND hWndParent, CUniString& str);

public:
	CVector<CDocument*>			m_Documents;
	CVector<CDocumentTemplate*>	m_Templates;

};

	} // namespace DocView

}	// namespace Simple

#endif	// __DOCUMENT_H

