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
// ResNode.cpp - implementation of CResNode class

#include "StdAfx.h"
#include "SimpleLibUtilsBuild.h"

#include "ResNode.h"
#include "ParseUtils.h"

namespace Simple
{

//////////////////////////////////////////////////////////////////////////
// CResNode

// Constructor
CResNode::CResNode() :
	m_pParentNode(NULL)
{
}

// Constructor
CResNode::CResNode(const CUniString& Name) :
	m_strName(Name),
	m_pParentNode(NULL)
{
}

// Copy constructor
CResNode::CResNode(const CResNode& other) :
	m_strName(other.m_strName),
	m_pParentNode(NULL)
{
	CopyFrom(&other);
}

// Destructor
CResNode::~CResNode()
{
}

CUniString CResNode::GetName() const
{
	return m_strName;
}

CUniString CResNode::GetFullName() const
{
	// If root, don't use name
	if (m_pParentNode==NULL)
		return L"";

	return Format(L"%s\\%s", m_pParentNode->GetFullName().sz(), m_strName.sz());
}



void CResNode::SetName(const CUniString& str)
{
	m_strName=str;
}

// Implementation of GetValueCount
int CResNode::GetValueCount() const
{
	return m_Values.GetSize();
}

// Implementation of HasValue
bool CResNode::HasValue(const wchar_t* pszName) const
{
	return m_Values.HasKey(pszName);
}

// Implementation of GetValue
CUniString CResNode::GetValue(const wchar_t* pszName, const wchar_t* pszDefault) const
{
	CUniString str;
	if (!m_Values.Find(pszName, str))
		return pszDefault;

	return str;
}

// Implementation of SetValue
void CResNode::SetValue(const CUniString& strName, const CUniString& strValue)
{
	m_Values.Add(strName, strValue);
}


int CResNode::GetIntValue(const wchar_t* pszName, int iDefault) const
{
	CUniString str;
	if (!m_Values.Get(pszName, str))
		return iDefault;

	int iValue;
	const wchar_t* pszValue=str;
	const wchar_t* psz=ParseInt(pszValue, &iValue);

	if (psz && (psz[0]==' ' || psz[0]=='\t' || psz[0]=='\0' || psz[0]==';'))
		return iValue;
	return iDefault;
}

void CResNode::SetIntValue(const CUniString& strName, int iValue)
{
	SetValue(strName, Format(L"%i", iValue));
}

int64_t CResNode::GetInt64Value(const wchar_t* pszName, int64_t iDefault) const
{
	CUniString str;
	if (!m_Values.Get(pszName, str))
		return iDefault;

	int64_t iValue;
	const wchar_t* pszValue=str;
	const wchar_t* psz=ParseInt64(pszValue, &iValue);
	if (psz && (psz[0]==' ' || psz[0]=='\t' || psz[0]=='\0' || psz[0]==';'))
		return iValue;

	return iDefault;
}

void CResNode::SetInt64Value(const CUniString& strName, int64_t iValue)
{
	SetValue(strName, Format(L"%I64i", iValue));
}

double CResNode::GetDoubleValue(const wchar_t* pszName, double dblDefault) const
{
	CUniString str;
	if (!m_Values.Get(pszName, str))
		return dblDefault;

	double dblValue;
	const wchar_t* psz=str;
	if (ReadDouble(psz, &dblValue) && (psz[0]==' ' || psz[0]=='\t' || psz[0]=='\0' || psz[0]==';'))
		return dblValue;

	return dblDefault;
}

void CResNode::SetDoubleValue(const CUniString& strName, double dbl)
{
	SetValue(strName, Format(L"%f", dbl));
}



// Implementation of GetValue
CUniString CResNode::GetValue(const wchar_t* pszSubNode, const wchar_t* pszName, const wchar_t* pszDefault) const
{
	CResNode* pNode=FindNode(pszSubNode);
	if (!pNode)
		return pszDefault;
	else
		return pNode->GetValue(pszName, pszDefault);
}

// Implementation of SetValue
void CResNode::SetValue(const CUniString& strSubNode, const CUniString& strName, const CUniString& strValue)
{
	CreateNode(strSubNode)->SetValue(strName, strValue);
}

int CResNode::GetIntValue(const wchar_t* pszSubNode, const wchar_t* pszName, int iDefault) const
{
	CResNode* pNode=FindNode(pszSubNode);
	if (!pNode)
		return iDefault;
	else
		return pNode->GetIntValue(pszName, iDefault);
}

void CResNode::SetIntValue(const CUniString& strSubNode, const CUniString& strName, int iValue)
{
	CreateNode(strSubNode)->SetIntValue(strName, iValue);
}

int64_t CResNode::GetInt64Value(const wchar_t* pszSubNode, const wchar_t* pszName, int64_t iDefault) const
{
	CResNode* pNode=FindNode(pszSubNode);
	if (!pNode)
		return iDefault;
	else
		return pNode->GetInt64Value(pszName, iDefault);
}

void CResNode::SetInt64Value(const CUniString& strSubNode, const CUniString& strName, int64_t iValue)
{
	CreateNode(strSubNode)->SetInt64Value(strName, iValue);
}

double CResNode::GetDoubleValue(const wchar_t* pszSubNode, const wchar_t* pszName, double dblDefault) const
{
	CResNode* pNode=FindNode(pszSubNode);
	if (!pNode)
		return dblDefault;
	else
		return pNode->GetDoubleValue(pszName, dblDefault);
}

void CResNode::SetDoubleValue(const CUniString& strSubNode, const CUniString& strName, double dblValue)
{
	CreateNode(strSubNode)->SetDoubleValue(strName, dblValue);
}


// Implementation of GetValueName
CUniString CResNode::GetValueName(int iIndex) const
{
	return m_Values[iIndex].Key;
}

CUniString CResNode::GetValue(int iIndex) const
{
	return m_Values[iIndex].Value;
}


// Implementation of DeleteValue
void CResNode::DeleteValue(const wchar_t* pszName)
{
	m_Values.Remove(pszName);
}

// Implementation of DeleteAllValues
void CResNode::DeleteAllValues()
{
	m_Values.RemoveAll();
}

// Implementation of GetNodeCount
int CResNode::GetNodeCount() const
{
	return m_Nodes.GetSize();
}

// Implementation of GetNode
CResNode* CResNode::GetNode(int iIndex) const
{
	return m_Nodes[iIndex]->ResolveLink();
}

// Implementation of FindNode
CResNode* CResNode::FindNode(const wchar_t* pszName) const
{
	for (int i=0; i<m_Nodes.GetSize(); i++)
	{
		if (IsEqualStringI(m_Nodes[i]->GetName(), pszName))
			return m_Nodes[i]->ResolveLink();
	}

	return NULL;
}

// Implementation of GetNodeNoResolve
CResNode* CResNode::GetNodeNoResolve(int iIndex) const
{
	return m_Nodes[iIndex];
}

// Implementation of FindNodeNoResolve
CResNode* CResNode::FindNodeNoResolve(const wchar_t* pszName) const
{
	for (int i=0; i<m_Nodes.GetSize(); i++)
	{
		if (IsEqualStringI(m_Nodes[i]->GetName(), pszName))
			return m_Nodes[i];
	}

	return NULL;
}

// Implementation of GetParentNode
CResNode* CResNode::GetParentNode() const
{
	return m_pParentNode;
}

// Implementation of GetRootNode
CResNode* CResNode::GetRootNode() const
{
	return m_pParentNode ? m_pParentNode->GetRootNode() : const_cast<CResNode*>(this);
}

// Find a node relative to this one
CResNode* CResNode::GetRelativeNode(const wchar_t* pszPath) const
{
	CResNode* pCurrent=const_cast<CResNode*>(this);

	if (!pszPath)
		return pCurrent;

	// Starting at root?
	const wchar_t* p=pszPath;
	if (p[0]=='\\')
	{
		pCurrent=GetRootNode();
		p++;
	}

	// Follow path
	while (p[0])
	{
		// Find next backslash
		const wchar_t* pszBSPos=wcschr(p, L'\\');

		// Get the element
		CUniString strElement(p, pszBSPos ? int(pszBSPos-p) : -1);

		CResNode* pNextNode;
		if (IsEqualString(strElement, L"."))
		{
			// Stay on this node
			pNextNode=pCurrent;
		}
		else if (IsEqualString(strElement, L".."))
		{
			// Move to parent
			pNextNode=pCurrent->GetParentNode();
		}
		else
		{
			// Find child node
			pNextNode=pCurrent->FindNode(strElement);
		}

		if (!pNextNode)
		{
			// Not found!
			return NULL;
		}

		// Move
		pCurrent=pNextNode;

		// Move to next element
		if (pszBSPos)
			p=pszBSPos+1;
		else
			break;
	}


	return pCurrent;
}



// Implementation of AddNode
int CResNode::AddNode(CResNode* pNode)
{
	InsertNode(-1, pNode);
	return m_Nodes.GetSize()-1;
}

// Implementation of InsertNode
void CResNode::InsertNode(int iIndex, CResNode* pNode)
{
	ASSERT(pNode->GetParentNode()==NULL);
	pNode->m_pParentNode=this;
	m_Nodes.Add(pNode);
}

// Implementation of CreateNode
CResNode* CResNode::CreateNode(const CUniString& strName)
{
	CResNode* pExisting=FindNode(strName);
	if (pExisting)
		return pExisting;
	else
		return CreateNewNode(strName);
}

// Implementation of CreateNewNode
CResNode* CResNode::CreateNewNode(const CUniString& strName)
{
	CResNode* pNode=new CResNode();
	pNode->SetName(strName);
	AddNode(pNode);
	return pNode;
}

// Implementation of DeleteNode
void CResNode::DeleteNode(int iIndex)
{
	m_Nodes.RemoveAt(iIndex);
}

// Implementation of DeleteNode
void CResNode::DeleteNode(CResNode* pNode)
{
	int iIndex=m_Nodes.Find(pNode);
	ASSERT(iIndex>=0);
	DeleteNode(iIndex);
}

// Implementatio of DeleteAllNodes
void CResNode::DeleteAllNodes()
{
	m_Nodes.RemoveAll();
}



// Implementation of DetachNode
CResNode* CResNode::DetachNode(int iIndex)
{
	CResNode* pNode=m_Nodes.DetachAt(iIndex);
	pNode->m_pParentNode=NULL;
	return pNode;
}

// Implementation of DetachNode
CResNode* CResNode::DetachNode(CResNode* pNode)
{
	m_Nodes.Detach(pNode);
	pNode->m_pParentNode=NULL;
	return pNode;
}

// Resolve link from this node to another
CResNode* CResNode::ResolveLink()
{
	CUniString strLink;
	if (!m_Values.Get(L"__link", strLink))
		return this;

	CResNode* pNode=GetRelativeNode(strLink);
	return pNode ? pNode : NULL;
}

// Set a link from the node to another
void CResNode::SetLink(const CUniString& strLink)
{
	SetValue(L"__link", strLink);
}

CUniString CResNode::GetLink()
{
	return GetValue(L"__link", NULL);
}


// Copy all values and sub-nodes from another node
void CResNode::CopyFrom(const CResNode* pOther)
{
	// Copy values
	for (int i=0; i<pOther->m_Values.GetSize(); i++)
	{
		m_Values.Add(pOther->m_Values[i].Key, pOther->m_Values[i].Value);
	}

	// Copy sub nodes
	for (int i=0; i<pOther->m_Nodes.GetSize(); i++)
	{
		AddNode(new CResNode(*pOther->m_Nodes[i]));
	}
}



}
