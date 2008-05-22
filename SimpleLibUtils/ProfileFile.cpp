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
// ProfileFile.cpp - implementation of CProfileFile class

#include "stdafx.h"
#include "SimpleLibUtilsBuild.h"

#include "ProfileFile.h"
#include "LoadSaveTextFile.h"
#include "ParseUtils.h"
#include "Wildcard.h"
#include "SplitString.h"
#include "Tokenizer.h"
#include "PathLibrary.h"
#include "StringReplace.h"

namespace Simple
{

//////////////////////////////////////////////////////////////////////////
// CProfileEntry

// Constructor
CProfileEntry::CProfileEntry(CProfileSection* pOwner, const CUniString& strName, const CUniString& strValue) : 
	m_pOwner(pOwner),
	m_strName(strName),
	m_strValue(strValue),
	m_lParam(0)
{
}

// Copy constructor
CProfileEntry::CProfileEntry(CProfileSection* pOwner, const CProfileEntry& Other) : 
	m_pOwner(pOwner),
	m_strName(Other.m_strName),
	m_strValue(Other.m_strValue),
	m_lParam(0)
{
}

const wchar_t* CProfileEntry::GetName() const
{
	return m_strName;
}

void CProfileEntry::SetName(const CUniString& strName)
{
	m_strName=strName;
}

const wchar_t* CProfileEntry::GetValue() const
{
	return m_strValue;
}

void CProfileEntry::SetValue(const CUniString& strValue)
{
	m_strValue=strValue;
}

const wchar_t* ParseInt(const wchar_t* psz, int* piValue)
{
	while (psz[0]==' ' || psz[0]=='\t')
		psz++;

	// Negative
	int iNeg=1;
	if (psz[0]==L'-')
		{
		iNeg=-1;
		psz++;
		}

	// Read integer
	if (!ReadInt(psz, piValue))
		return NULL;

	// Parse it
	*piValue*=iNeg;
	return psz;
}



int CProfileEntry::GetIntValue(int iDefault) const
{
	if (!this)
		return iDefault;

	int iValue;
	const wchar_t* psz=ParseInt(GetValue(), &iValue);
	if (psz && (psz[0]==' ' || psz[0]=='\t' || psz[0]=='\0' || psz[0]==';'))
		return iValue;

	return iDefault;
}

void CProfileEntry::SetIntValue(int iValue)
{
	SetValue(Format(L"%i", iValue));
}

double CProfileEntry::GetDoubleValue(double dblDefault) const
{
	if (!this)
		return dblDefault;

	double dblValue;
	const wchar_t* psz=GetValue();
	if (ReadDouble(psz, &dblValue) && psz[0]==' ' || psz[0]=='\t' || psz[0]=='\0' || psz[0]==';')
		return dblValue;

	return dblDefault;
}

void CProfileEntry::SetDoubleValue(double dbl)
{
	SetValue(Format(L"%f", dbl));
}


size_t CProfileEntry::GetItemData() const
{
	return m_lParam;
}

void CProfileEntry::SetItemData(size_t lParam)
{
	m_lParam=lParam;
}

CProfileFile* CProfileEntry::GetOwningFile()
{ 
	return m_pOwner ? m_pOwner->GetOwningFile() : NULL; 
}


//////////////////////////////////////////////////////////////////////////
// CProfileSection

// Constructor
CProfileSection::CProfileSection(CProfileOwner* pOwner, const CUniString& strName, bool bFlat) : 
	m_pOwner(pOwner),
	m_strName(strName),
	m_lParam(0),
	m_bFlat(bFlat)
{
}

// Copy constructor
CProfileSection::CProfileSection(CProfileOwner* pOwner, const CProfileSection& Other) : 
	m_pOwner(pOwner),
	m_strName(Other.m_strName),
	m_bFlat(Other.m_bFlat),
	m_lParam(0)
{
	CopyFrom(&Other);
}



const wchar_t* CProfileSection::GetName() const
{
	if (!this)
		return NULL;

	return m_strName;
}

void CProfileSection::SetName(const CUniString& strName)
{
	m_strName=strName;
}

int CProfileSection::FindEntryIndexRaw(const wchar_t* pszName, int iStartAfter) const
{
	if (!this)
		return -1;

	for (int i=iStartAfter+1; i<GetSize(); i++)
		{
		if (!IsEmptyString(GetAt(i)->GetName()) && DoesWildcardMatch(pszName, GetAt(i)->GetName()))
			return i;
		}

	return -1;
}

int CProfileSection::FindEntryIndex(const wchar_t* pszName, int iStartAfter) const
{
	const wchar_t* pszMode=m_pOwner->GetModeInternal();
	if (pszMode)
	{
		int iRetv=FindEntryIndexRaw(Format(L"%s.%s", pszName, pszMode), iStartAfter);
		if (iRetv>=0)
			return iRetv;
	}

	return FindEntryIndexRaw(pszName, iStartAfter);
}

CProfileEntry* CProfileSection::FindEntry(const wchar_t* pszName) const
{
	int iIndex=FindEntryIndex(pszName, -1);
	if (iIndex<0)
		return NULL;
	return GetAt(iIndex);
}


size_t CProfileSection::GetItemData() const
{
	return m_lParam;
}

void CProfileSection::SetItemData(size_t size_t)
{
	m_lParam=size_t;
}

CProfileEntry* CProfileSection::CreateEntry(const CUniString& strName, const CUniString& strValue)
{
	CProfileEntry* pEntry=new CProfileEntry(this, strName, strValue);
	Add(pEntry);
	return pEntry;
}


int CProfileSection::FindSectionIndexRaw(const wchar_t* pszName, int iStartAfter) const
{
	if (!this)
		return -1;

	for (int i=iStartAfter+1; i<m_Sections.GetSize(); i++)
		{
		if (!IsEmptyString(m_Sections[i]->GetName()) && DoesWildcardMatch(pszName, m_Sections[i]->GetName()))
			return i;
		}

	return -1;
}

int CProfileSection::FindSectionIndex(const wchar_t* pszName, int iStartAfter) const
{
	if (!this)
		return -1;

	const wchar_t* pszMode=m_pOwner->GetModeInternal();
	if (pszMode)
	{
		int iRetv=FindSectionIndexRaw(Format(L"%s.%s", pszName, pszMode), iStartAfter);
		if (iRetv>=0)
			return iRetv;
	}

	return FindSectionIndexRaw(pszName, iStartAfter);
}

bool CProfileSection::DeleteSection(const wchar_t* pszName)
{
	CUniString strSection, strChild;
	if (m_bFlat)
		strSection=pszName;
	else
		SplitString(pszName, L"\\", strSection, strChild);

	int iIndex=FindSectionIndex(strSection);
	if (iIndex<0)
		return false;

	if (strChild.IsEmpty())
	{
		m_Sections.RemoveAt(iIndex);
	}
	else
	{
		return m_Sections[iIndex]->DeleteSection(strChild);
	}
	return true;
}

CProfileSection* CProfileSection::FindSection(const wchar_t* pszName) const
{
	CUniString strSection, strChild;
	if (m_bFlat)
		strSection=pszName;
	else
		SplitString(pszName, L"\\", strSection, strChild);

	int iIndex=FindSectionIndex(strSection);
	if (iIndex<0)
		return NULL;

	if (strChild.IsEmpty())
	{
		return m_Sections[iIndex];
	}
	else
	{
		return m_Sections[iIndex]->FindSection(strChild);
	}
}

CProfileSection* CProfileSection::CreateSection(const wchar_t* pszName)
{
	CUniString strSection, strChild;
	if (m_bFlat)
		strSection=pszName;
	else
		SplitString(pszName, L"\\", strSection, strChild);

	// Find existing section...
	CProfileSection* pSection=FindSection(strSection);
	if (!pSection)
	{
		// Create new section
		pSection=new CProfileSection(this, pszName, m_bFlat);
		m_Sections.Add(pSection);
	}

	if (strChild.IsEmpty())
		return pSection;
	else
		return pSection->CreateSection(pszName);
}

int CProfileSection::GetSubSectionCount() const
{
	return m_Sections.GetSize();
}

CProfileSection* CProfileSection::GetSubSection(int iIndex) const
{
	return m_Sections[iIndex];
}

void CProfileSection::CopyFrom(const CProfileSection* pSection)
{
	// Copy values
	for (int i=0; i<pSection->GetSize(); i++)
	{
		Add(new CProfileEntry(this, *pSection->GetAt(i)));
	}

	// Copy subsections
	for (int i=0; i<pSection->GetSubSectionCount(); i++)
	{
		m_Sections.Add(new CProfileSection(this, *pSection->GetSubSection(i)));
	}
}


bool CProfileSection::DeleteValue(const wchar_t* pszName)
{
	int iIndex=FindEntryIndex(pszName);
	if (iIndex>=0)
	{
		RemoveAt(iIndex);
		return true;
	}

	return false;
}


void CProfileSection::SetValue(const CUniString& strName, const CUniString& strValue)
{
	CProfileEntry* pEntry=FindEntry(strName);
	if (pEntry)
		pEntry->SetValue(strValue);
	else
		CreateEntry(strName, strValue);
}

const wchar_t* CProfileSection::GetValue(const wchar_t* pszName, const wchar_t* pszDefault) const
{
	CProfileEntry* pEntry=FindEntry(pszName);
	if (!pEntry)
		return pszDefault;
	else
		return pEntry->GetValue();
}

void CProfileSection::SetIntValue(const wchar_t* pszName, int iValue)
{
	CProfileEntry* pEntry=FindEntry(pszName);
	if (pEntry)
		pEntry->SetIntValue(iValue);
	else
		CreateEntry(pszName)->SetIntValue(iValue);
}

int CProfileSection::GetIntValue(const wchar_t* pszName, int iDefault) const
{
	CProfileEntry* pEntry=FindEntry(pszName);
	if (!pEntry)
		return iDefault;
	else
		return pEntry->GetIntValue(iDefault);
}


void CProfileSection::SetDoubleValue(const wchar_t* pszName, double dblValue)
{
	CProfileEntry* pEntry=FindEntry(pszName);
	if (pEntry)
		pEntry->SetDoubleValue(dblValue);
	else
		CreateEntry(pszName)->SetDoubleValue(dblValue);
}

double CProfileSection::GetDoubleValue(const wchar_t* pszName, double dblDefault) const
{
	CProfileEntry* pEntry=FindEntry(pszName);
	if (!pEntry)
		return dblDefault;
	else
		return pEntry->GetDoubleValue(dblDefault);
}



//////////////////////////////////////////////////////////////////////////
// CProfileFile

// Constructor
CProfileFile::CProfileFile()
{
	m_bFlat=true;
}

// Destructor
CProfileFile::~CProfileFile()
{
}

void CProfileFile::Define(const wchar_t* pszName, const wchar_t* pszValue)
{
	m_ExternalDefines.Add(pszName, pszValue);
}

void CProfileFile::Reset(bool bFlat)
{
	m_bFlat=bFlat;
	RemoveAll();
}

bool IsClassicProfile(const wchar_t* p)
{
	// "Classic" profile files start with optional whitespace and either section name or comment...
	SkipWhiteSpace(p);
	if (p[0]=='[')
		return true;
	if (p[0]==';')
		return true;

	// Assume its a "structured" profile file...
	return false;
}

bool CProfileFile::Parse(const wchar_t* pszContent, const wchar_t* pszFileName, CContentProvider* pContentProvider)
{
	if (IsClassicProfile(pszContent))
	{
		return ParseClassic(pszContent);
	}
	else
	{
		return ParseStructured(pszContent, pszFileName, pContentProvider);
	}
}

bool CProfileFile::ParseClassic(const wchar_t* pszContent)
{
	// Set flat flat
	m_bFlat=true;

	// Parse file...
	const wchar_t* p=pszContent;

	// Parse sections...
	CProfileSection* pSection;
	while (p[0])
		{
		// Skip leading white space
		SkipWhiteSpace(p);

		// Is it a section
		if (p[0]=='[')
			{
			// Parse section header...
			p++;
			const wchar_t* pszSection=p;
			while (!IsEOL(p[0]) && p[0] && p[0]!=']')
				p++;
			
			// End of section header?
			if (p[0]!=']')
				{
				RemoveAll();
				return false;
				}

			// Create new section
			CUniString strSection(pszSection, int(p-pszSection));

			// Split section name, creating sub sections.  Separator double backslash
			const wchar_t* pszSep;
			CProfileSection* pParentSection=NULL;
			while ((pszSep=wcsstr(strSection, L"\\\\"))!=NULL)
			{
				// Split it
				CUniString strParent(strSection, int(pszSep-static_cast<const wchar_t*>(strSection)));
				CUniString strChild=pszSep+2;

				// Get parent section
				pParentSection=pParentSection ? pParentSection->CreateSection(strParent) : CreateSection(strParent);

				// Keep splitting...
				strSection=strChild;
			}


			// Create new section
			pSection=pParentSection ? pParentSection->CreateSection(strSection) : CreateSection(strSection);

			// Skip to next line
			SkipToNextLine(p);
			continue;

			}

		// Comment?
		if (p[0]==';')
			{
			// Find end of comment
			SkipToNextLine(p);
			continue;
			}

		// Store start of name
		const wchar_t* pszName=p;

		// Find end of name
		while (!IsEOL(p[0]) && p[0] && p[0]!='=')
			p++;

		// Trim name
		CUniString strName(pszName, int(p-pszName));

		// Create entry...
		CProfileEntry* pEntry=new CProfileEntry(pSection, strName);

		// Parse assignment
		if (p[0]=='=')
			{
			CUniString buf;

			// Read lines
			while (p[0]=='=')
				{
				p++;

				// Store value
				const wchar_t* pszValue=p;

				// Find end of value
				while (!IsEOL(p[0]) && p[0])
					p++;

				// Append value...
				buf.Append(pszValue, int(p-pszValue));

				// Skip the eol
				SkipEOL(p);

				// Multiline?
				if (p[0]=='=')
					buf.Append(L"\r\n");
				}

			// Store it...
			pEntry->SetValue(buf);
			}

		if (pSection)
			pSection->Add(pEntry);
		else
			delete pEntry;
		}

	return true;
}


enum
{
	tokenOpenBrace=tokenOperatorBase,
	tokenCloseBrace,
	tokenEquals,
	tokenSemiColon,
	tokenPointer,
	tokenOr,
};

// Parse a structured profile file
bool CProfileFile::ParseStructured(const wchar_t* pszContent, const wchar_t* pszFileName, CContentProvider* pContentProvider)
{
	// Set flat flat
	m_bFlat=false;

	// Setup tokenizer
	CCppTokenizer tokens;
	tokens.SetContentProvider(pContentProvider);
	tokens.SetExtraIdentifierChars(NULL, L".");
	tokens.AddOperator(L"{", tokenOpenBrace);
	tokens.AddOperator(L"}", tokenCloseBrace);
	tokens.AddOperator(L"=", tokenEquals);
	tokens.AddOperator(L";", tokenSemiColon);
	tokens.AddOperator(L"->", tokenPointer);
	tokens.AddOperator(L"|", tokenOr);

	for (int i=0; i<m_ExternalDefines.GetSize(); i++)
	{
		tokens.Define(m_ExternalDefines[i].Key, m_ExternalDefines[i].Value);
	}

	// Initialize tokenizer
	tokens.ParseString(pszContent, pszFileName);

	if (ParseStructuredContent(tokens))
		return true;

	CUniString strErrorFile;
	int iLine=tokens.GetCurrentPosition(strErrorFile);
	m_strParseError=tokens.GetError();
	return false;
}

bool CProfileFile::ParseStructuredContent(CCppTokenizer& tokens)
{
	CIndex<CProfileSection*, CUniString>	Links;
	// Parse it
	CVector<CProfileSection*>	ScopeStack;
	while (tokens.CurrentToken()!=tokenEOF)
	{
		// Ending a section
		if (tokens.CurrentToken()==tokenCloseBrace)
		{
			// Skip it
			tokens.NextToken();

			if (tokens.CurrentToken()==tokenSemiColon)
			{
				tokens.NextToken();
			}

			// Check have something to pop!
			if (ScopeStack.IsEmpty())
			{
				tokens.Unexpected();
				return false;
			}

			// Pop scope stack...
			ScopeStack.Pop();
			continue;
		}

		// Get name of element - expect string literal or identifier...
		CUniString strName=tokens.GetStringLiteral();
		if (tokens.CurrentToken()!=tokenStringLiteral && tokens.CurrentToken()!=tokenIdentifier)
		{
			tokens.Unexpected(L"when parsing section or value name ");
			return false;
		}
		tokens.NextToken();

		if (tokens.CurrentToken()==tokenPointer)
		{
			// Get the link to name
			tokens.NextToken();
			CUniString strLinkTo=tokens.GetStringLiteral();
			if (!tokens.Skip(tokenStringLiteral))
				return false;
			if (strLinkTo.IsEmpty())
			{
				tokens.SetError(L"Missing link reference name");
				return false;
			}


			// Work out full path to this section...
			if (strLinkTo[0]=='.')
			{
				CUniString strThisSection;
				for (int i=0; i<ScopeStack.GetSize(); i++)
				{
					if (i>0)
						strThisSection+=L"\\";
					strThisSection+=ScopeStack[i]->m_strName;
				}

				strLinkTo=CanonicalPathAppend(strThisSection, strLinkTo);
			}

			// Start a new section
			CProfileSection* pNewSection;
			if (ScopeStack.IsEmpty())
			{
				pNewSection=CreateSection(strName);
			}
			else
			{
				pNewSection=ScopeStack.Top()->CreateSection(strName);
			}


			CProfileSection* pLinkTo=FindSection(strLinkTo);
			if (!pLinkTo)
			{
				tokens.SetError(Format(L"Unable to resolve link to '%s'", strLinkTo));
				return false;
			}

			pNewSection->CopyFrom(pLinkTo);

			tokens.Skip(tokenSemiColon);
			continue;
		}

		// Parse an optional assign before open brace...
		bool bHaveAssign=tokens.CurrentToken()==tokenEquals;
		if (bHaveAssign)
		{
			tokens.NextToken();
		}

		// Starting a section?
		if (tokens.CurrentToken()==tokenOpenBrace)
		{
			// Skip it
			tokens.NextToken();

			// Start a new section
			if (ScopeStack.IsEmpty())
			{
				ScopeStack.Push(CreateSection(strName));
			}
			else
			{
				ScopeStack.Push(ScopeStack.Top()->CreateSection(strName));
			}
			continue;
		}

		// Expect assignment
		if (!bHaveAssign)
		{
			if (!tokens.Skip(tokenEquals))
				return false;
			tokens.NextToken();
		}

		// Check have a section...
		if (ScopeStack.IsEmpty())
		{
			tokens.SetError(L"Syntax error - value not expected outside section braces");
			return false;
		}

		// Parse value...
		CUniString strValue;
		switch (tokens.CurrentToken())
		{
			case tokenStringLiteral:
			case tokenIdentifier:
				strValue=tokens.GetStringLiteral();
				tokens.NextToken();
				break;

			case tokenDoubleLiteral:
				strValue=tokens.GetTokenText();
				tokens.NextToken();
				break;

			case tokenInt32Literal:
				if (tokens.GetTypeQualifier()==tqRgb)
				{
					strValue=tokens.GetTokenText();
					tokens.NextToken();
				}
				else
				{
					int iValue=tokens.GetInt32Literal();
					tokens.NextToken();
					while (tokens.CurrentToken()==tokenOr)
					{
						tokens.NextToken();
						if (!tokens.Check(tokenInt32Literal))
							return false;
						iValue|=tokens.GetInt32Literal();
						tokens.NextToken();
					}
					
					strValue=Format(L"%i", iValue);
				}
				break;

			case tokenInt64Literal:
				strValue=Format(L"%I64i", tokens.GetInt64Literal());
				tokens.NextToken();
				break;

			default:
				tokens.Unexpected(L"when parsing value ");
				return false;
		}

		// Skip semicolon
		if (!tokens.Skip(tokenSemiColon))
			return false;

		// Store value...
		ScopeStack.Top()->SetValue(strName, strValue);

	}

	// Check nothing left on stack (unclosed brace)
	if (!ScopeStack.IsEmpty())
	{
		tokens.SetError(L"Missing closing brace");
		return false;
	}

	// Done!
	return true;
}

CUniString CProfileFile::GetParseError() const
{
	return m_strParseError;
}



bool CProfileFile::Load(const wchar_t* pszFileName, CContentProvider* pContentProvider)
{
	Reset(true);

	// Setup a default file provider...
	CFileContentProvider fileprovider;
	if (!pContentProvider)
	{
		pContentProvider=&fileprovider;
	}

	// Get file content...
	CUniString str;
	if (!pContentProvider->GetFileContent(NULL, pszFileName, false, m_strFileName, str))
	{
		m_strParseError=Format(L"Failed to load %s", pszFileName);
		return false;
	}

	// Parse it
	return Parse(str, m_strFileName, pContentProvider);
}

bool CProfileFile::Save(const wchar_t* pszFileName, bool bUnicode, bool bHeirarchial)
{
	CUniString buf;
	SaveToString(buf, bHeirarchial);

	// New file name?
	if (pszFileName)
		m_strFileName=pszFileName;

	if (bUnicode)
	{
		if (!SaveTextFile(m_strFileName, buf))
			return false;
	}
	else
	{
		if (!SaveTextFile(m_strFileName, w2a(buf)))
			return false;
	}

	// Done
	return true;
}


bool CProfileFile::SaveToString(CUniString& buf, bool bHeirarchial) const
{
	if (!bHeirarchial)
	{
		for (int i=0; i<GetSize(); i++)
		{
			WriteSection(buf, GetAt(i), L"");
		}
	}
	else
	{
		int iIndent=0;
		for (int i=0; i<GetSize(); i++)
		{
			WriteSectionHeirarchial(buf, GetAt(i), iIndent);
		}
	}

	return true;
}

int CProfileFile::FindSectionIndexRaw(const wchar_t* pszName, int iStartAfter) const
{
	for (int i=iStartAfter+1; i<GetSize(); i++)
		{
		if (!IsEmptyString(GetAt(i)->GetName()) && DoesWildcardMatch(pszName, GetAt(i)->GetName()))
			return i;
		}

	return -1;
}

int CProfileFile::FindSectionIndex(const wchar_t* pszName, int iStartAfter) const
{
	if (!this)
		return -1;

	if (!m_strMode.IsEmpty())
	{
		int iRetv=FindSectionIndexRaw(Format(L"%s.%s", pszName, m_strMode), iStartAfter);
		if (iRetv>=0)
			return iRetv;
	}

	return FindSectionIndexRaw(pszName, iStartAfter);
}



void CProfileFile::Merge(CProfileFile* pOther)
{
	for (int i=0; i<pOther->GetSize(); i++)
		{
		CProfileSection* pSection=pOther->GetAt(i);
		Merge(pOther->GetAt(i));
		}
}

void CProfileFile::Merge(CProfileSection* pSection)
{
	CProfileSection* pDest=CreateSection(pSection->GetName());

	for (int i=0; i<pSection->GetSize(); i++)
		{
		CProfileEntry* pEntry=pSection->GetAt(i);
		if (!IsEmptyString(pEntry->GetName()))
			pDest->SetValue(pEntry->GetName(), pEntry->GetValue());
		}
}


CProfileSection* CProfileFile::FindSection(const wchar_t* pszName) const
{
	CUniString strSection, strChild;
	if (m_bFlat)
		strSection=pszName;
	else
		SplitString(pszName, L"\\", strSection, strChild);

	int iIndex=FindSectionIndex(strSection, -1);
	if (iIndex<0)
		return NULL;

	if (strChild.IsEmpty())
		return GetAt(iIndex);
	else
		return GetAt(iIndex)->FindSection(strChild);
}

bool CProfileFile::DeleteSection(const wchar_t* pszName)
{
	CUniString strSection, strChild;
	if (m_bFlat)
		strSection=pszName;
	else
		SplitString(pszName, L"\\", strSection, strChild);

	int iIndex=FindSectionIndex(strSection, -1);
	if (iIndex<0)
		return false;

	if (strChild.IsEmpty())
	{
		RemoveAt(iIndex);
		return true;
	}
	else
	{
		return GetAt(iIndex)->DeleteSection(strChild);
	}
}

// Create a new section (returning existing one if already exists)
CProfileSection* CProfileFile::CreateSection(const CUniString& strName)
{
	CUniString strSection, strChild;
	if (m_bFlat)
		strSection=strName;
	else
		SplitString(strName, L"\\", strSection, strChild);

	// Find existing section...
	CProfileSection* pSection=FindSection(strSection);
	if (!pSection)
	{
		// Create new section
		pSection=new CProfileSection(this, strName, m_bFlat);
		Add(pSection);
	}

	if (strChild.IsEmpty())
	{
		return pSection;
	}
	else
	{
		return pSection->CreateSection(strChild);
	}
}

CProfileEntry* CProfileFile::FindEntry(const wchar_t* pszSection, const wchar_t* pszEntry) const
{
	CProfileSection* pSection=FindSection(pszSection);
	if (!pSection)
		return NULL;

	return pSection->FindEntry(pszEntry);
}

CProfileEntry* CProfileFile::CreateEntry(const CUniString& strSection, const CUniString& strEntry)
{
	return CreateSection(strSection)->CreateEntry(strEntry);
}

void CProfileFile::WriteEntry(CUniString& buf, CProfileEntry* pEntry) const
{
	if (!IsEmptyString(pEntry->m_strName))
		{
		buf.Append(pEntry->m_strName);
		buf.Append(L"=");
		}

	if (!IsEmptyString(pEntry->m_strValue))
		{
		const wchar_t* p=pEntry->m_strValue;
		while (p[0])
			{
			// Save start of line
			const wchar_t* pszLine=p;

			// Find end of line
			while (p[0] && !IsEOL(p[0]))
				p++;

			// Append line
			buf.Append(pszLine, int(p-pszLine));

			// If continued line, append cr/lf and = continuation character
			if (p[0])
				{
				buf.Append(L"\r\n=");
				SkipEOL(p);
				}
			}

		}

	
	buf.Append(L"\r\n");

}

void CProfileFile::WriteSection(CUniString& buf, CProfileSection* pSection, const wchar_t* pszParent) const
{
	// Write section header
	if (!IsEmptyString(pSection->m_strName))
		{
		buf.Append(L"[");
		buf.Append(pszParent);
		buf.Append(pSection->m_strName);
		buf.Append(L"]\r\n");
		}

	// Write all values...
	for (int i=0; i<pSection->GetSize(); i++)
		{
		WriteEntry(buf, pSection->GetAt(i));
		}

	// Blank line...
	buf.Append(L"\r\n");

	// Write sub sections...
	for (int i=0; i<pSection->m_Sections.GetSize(); i++)
	{
		WriteSection(buf, pSection->m_Sections[i], Format(L"%s%s\\\\", pszParent, pSection->m_strName));
	}
}

const wchar_t pszTabs[]=L"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t";

CUniString EscapeString(const wchar_t* psz, bool bValue)
{
	if (IsEmptyString(psz))
		return L"\"\"";

	if (bValue)
	{
		// Check if all digits
		const wchar_t* p=psz;
		while (p[0])
		{
			if (!iswdigit(p[0]))
				break;
			p++;
		}

		// Yes!
		if (!p[0])
			return psz;
	}

	CUniString str;

	bool bNeedQuotes=!!iswdigit(psz[0]);

	const wchar_t* p=psz;
	while (p[0])
	{
		if (!iswalnum(p[0]))
			bNeedQuotes=true;

		switch (p[0])
		{
			case L'\\':
				str+=L"\\\\";
				break;

			case L'\n':
				str+=L"\\n";
				break;

			case L'\r':
				str+=L"\\n";
				break;

			case L'\t':
				str+=L"\\t";
				break;

			default:
				str.Append(p, 1);
				break;
		}

		p++;
	}

	if (bNeedQuotes)
		str=Format(L"\"%s\"", str);

	return str;
}

void CProfileFile::WriteSectionHeirarchial(CUniString& buf, CProfileSection* pSection, int iIndent) const
{
	buf.Append(pszTabs, iIndent);
	buf.Append(pSection->GetName());
	buf.Append(L"\r\n");
	buf.Append(pszTabs, iIndent);
	buf.Append(L"{\r\n");

	// Write values
	for (int i=0; i<pSection->GetSize(); i++)
	{
		buf.Append(pszTabs, iIndent+1);
		buf.Append(EscapeString(pSection->GetAt(i)->GetName(), false));
		buf.Append(L"=");
		buf.Append(EscapeString(pSection->GetAt(i)->GetValue(), true));
		buf.Append(L";\r\n");
	}

	// Write sub sections
	for (int i=0; i<pSection->GetSubSectionCount(); i++)
	{
		WriteSectionHeirarchial(buf, pSection->GetSubSection(i), iIndent+1);
	}

	buf.Append(pszTabs, iIndent);
	buf.Append(L"}\r\n");
}


bool CProfileFile::DeleteValue(const wchar_t* pszSection, const wchar_t* pszName)
{
	CProfileSection* pSection=FindSection(pszSection);
	if (!pSection)
		return false;

	return pSection->DeleteValue(pszName);
}

void CProfileFile::SetValue(const CUniString& strSection, const CUniString& strName, const CUniString& strValue)
{
	CreateSection(strSection)->SetValue(strName, strValue);
}

const wchar_t* CProfileFile::GetValue(const wchar_t* pszSection, const wchar_t* pszName, const wchar_t* pszDefault) const
{
	CProfileSection* pSection=FindSection(pszSection);
	if (!pSection)
		return pszDefault;
	else
		return pSection->GetValue(pszName, pszDefault);
}

void CProfileFile::SetIntValue(const CUniString& strSection, const CUniString& strName, int iValue)
{
	CreateSection(strSection)->SetIntValue(strName, iValue);
}

int CProfileFile::GetIntValue(const wchar_t* pszSection, const wchar_t* pszName, int iDefault) const
{
	CProfileSection* pSection=FindSection(pszSection);
	if (!pSection)
		return iDefault;
	else
		return pSection->GetIntValue(pszName, iDefault);
}

void CProfileFile::SetDoubleValue(const CUniString& strSection, const CUniString& strName, double dblValue)
{
	CreateSection(strSection)->SetDoubleValue(strName, dblValue);
}

double CProfileFile::GetDoubleValue(const wchar_t* pszSection, const wchar_t* pszName, double dblDefault) const
{
	CProfileSection* pSection=FindSection(pszSection);
	if (!pSection)
		return dblDefault;
	else
		return pSection->GetDoubleValue(pszName, dblDefault);
}

void CProfileFile::SetMode(const wchar_t* pszMode)
{
	m_strMode=pszMode;
}

CUniString CProfileFile::GetMode()
{
	return m_strMode;
}





}	// namespace Simple
