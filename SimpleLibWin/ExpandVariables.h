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
// ExpandVariablesEx.h - declaration of ExpandVariablesEx

#ifndef __EXPANDVARIABLESEX_H
#define __EXPANDVARIABLESEX_H

namespace Simple
{

// Implement to resolve variable names to values
struct IVariableProvider
{
	virtual bool ResolveVariable(const wchar_t* psz, CUniString& str)=0;
};

// Resolved special paths eg: $(CSIDL_DESKTOP)
class CSpecialPathVariableProvider : public IVariableProvider
{
public:
	virtual bool ResolveVariable(const wchar_t* psz, CUniString& str);

	static CSpecialPathVariableProvider* GetInstance();
};

// Resolves environment string variables eg: $(include)
class CEnvironmentVariableProvider : public IVariableProvider
{
public:
	virtual bool ResolveVariable(const wchar_t* psz, CUniString& str);

	static CEnvironmentVariableProvider* GetInstance();
};

// Resolves registry strings eg: $(HKLM\Software\CompanyName\ProductName\InstallDir)
// If variable ends in \ treats as default key value.
// Otherwise last component is registry value name
class CRegistryVariableProvider : public IVariableProvider
{
public:
	virtual bool ResolveVariable(const wchar_t* psz, CUniString& str);
	static CRegistryVariableProvider* GetInstance();
};

// Composite variable provider calls N other variable provders
class CCompositeVariableProvider : public IVariableProvider
{
public:
// Operations
	void AddProvider(IVariableProvider* pProvider);

// IVariableProvider
	virtual bool ResolveVariable(const wchar_t* psz, CUniString& str);

protected:
	CVector<IVariableProvider*>	m_Providers;

};

CUniString ExpandVariables(const wchar_t* psz, IVariableProvider* pProvider);

}	// namespace Simple

#endif	// __EXPANDVARIABLESEX_H

