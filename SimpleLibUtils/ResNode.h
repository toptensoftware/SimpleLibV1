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
// ResNode.h - declaration of CResNode class

#ifndef __RESNODE_H
#define __RESNODE_H


namespace Simple
{

// CResNode Class
class CResNode
{
public:
// Construction
			CResNode();
			CResNode(const CUniString& Name);
			CResNode(const CResNode& other);
	virtual ~CResNode();

	CUniString GetName() const;
	CUniString GetFullName() const;
	void SetName(const CUniString& str);

// Values
	int GetValueCount() const;
	bool HasValue(const wchar_t* pszName) const;
	CUniString GetValueName(int iIndex) const;
	CUniString GetValue(int iIndex) const;
	void DeleteValue(const wchar_t* pszName);
	void DeleteAllValues();

// Direct values
	CUniString GetValue(const wchar_t* pszName, const wchar_t* pszDefault) const;
	void SetValue(const CUniString& strName, const CUniString& strValue);
	int GetIntValue(const wchar_t* pszName, int iDefault) const;
	void SetIntValue(const CUniString& strName, int iValue);
	int64_t GetInt64Value(const wchar_t* pszName, int64_t iDefault) const;
	void SetInt64Value(const CUniString& strName, int64_t iValue);
	double GetDoubleValue(const wchar_t* pszName, double dblDefault) const;
	void SetDoubleValue(const CUniString& strName, double dblValue);

// Sub-node values
	CUniString GetValue(const wchar_t* pszSubNode, const wchar_t* pszName, const wchar_t* pszDefault) const;
	void SetValue(const CUniString& strSubNode, const CUniString& strName, const CUniString& strValue);
	int GetIntValue(const wchar_t* pszSubNode, const wchar_t* pszName, int iDefault) const;
	void SetIntValue(const CUniString& strSubNode, const CUniString& strName, int iValue);
	int64_t GetInt64Value(const wchar_t* pszSubNode, const wchar_t* pszName, int64_t iDefault) const;
	void SetInt64Value(const CUniString& strSubNode, const CUniString& strName, int64_t iValue);
	double GetDoubleValue(const wchar_t* pszSubNode, const wchar_t* pszName, double dblDefault) const;
	void SetDoubleValue(const CUniString& strSubNode, const CUniString& strName, double dbl);

// Sub node
	int GetNodeCount() const;
	CResNode* GetNode(int iIndex) const;
	CResNode* FindNode(const wchar_t* pszName) const;
	CResNode* GetNodeNoResolve(int iIndex) const;
	CResNode* FindNodeNoResolve(const wchar_t* pszName) const;
	CResNode* GetParentNode() const;
	CResNode* GetRootNode() const;
	CResNode* GetRelativeNode(const wchar_t* pszPath) const;
	int AddNode(CResNode* pNode);
	void InsertNode(int iIndex, CResNode* pNode);
	CResNode* CreateNode(const CUniString& strName);
	CResNode* CreateNewNode(const CUniString& strName);
	void DeleteNode(int iIndex);
	void DeleteNode(CResNode* pNode);
	void DeleteAllNodes();
	CResNode* DetachNode(int iIndex);
	CResNode* DetachNode(CResNode* pNode);
	CResNode* ResolveLink();
	void SetLink(const CUniString& strLink);
	CUniString GetLink();

// Utility
	void CopyFrom(const CResNode* pOther);

protected:
// Values
	CUniString						m_strName;
	CResNode*						m_pParentNode;
	CMap<CUniString, CUniString>	m_Values;
	CVector<CResNode*, SOwnedPtr>	m_Nodes;

// Operations

};

}

#endif	// __RESNODE_H

