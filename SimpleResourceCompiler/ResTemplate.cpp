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
// ResTemplate.cpp - implementation of CResNode class

#include "StdAfx.h"

#include "ResTemplate.h"

namespace Simple
{

/*
Template Syntax
---------------

{=<expr>}		// Searches current node and all parents...

{for:<expr>}	// Repeats content for all elements in expression
	--repeated--
{else}
	--emptyloop--
{/for}


{if:<expr>}		// Conditional
	--true--
{else}
	--false--
{/if}

{with:<expr>}
{/with}


<expr> := 
	@<identifier>		Looks for an attribute on the current node, or and parents
	@"string"			Looks for an attribute on the current node, or and parents
	@					Name of the current node



Functions
=========
[node.]@<attribute>			- value of an attribute
[node.]hasattrib(attribname)- check if node has an attribute
[node.]children[wild]		- all child nodes of the current element
[node.]allchildren[wild]	- all child nodes of the current element recursive
[node.]attributes[wild]		- all attributes of a node
[node.]first				- first child in a collection of nodes
[node.]last					- last child in a collection of nodes
[nodes.]select(wild)		- select nodes matching a wildcard
[node.]parent				- parent of current node
[loopctx].isfirst			- true if in the first loop iteration
[loopctx].islast			- true if in the last loop iteration
[loopctx].isinternal		- true if in internal loop iteration
[loopctx].outerloop			- returns the outer loop context
[loopctx].currentelement	- returns the current iteration element
[loopctx].looppos			- current loop iteration index
[loopctx].loopcount			- loop count
[nodectx].outernode			- returns the outer node context
indent						- indent all subsequent lines by 1 additional indent
unindent					- end one level of indentation
string.htmlescape			- escape with html characters
string.cppstring			- output as a C++ string
string.srlescape			- escape string to be accepted by SRL compiler

Special variables;
	- this - returns the current node being enumerated
	- iter - returns the current loop iterator

outernode.@name

{def:myfunction}
{indent}
{for:children}
{myfunction}
{/for}
{unindent}
{/def}

{for:children("event")}
   {if:isfirst}
	<table>
   {/if}
   <tr>{=@Name}</tr>
   {myfunction}
   {if:islast}
	</table>
   {/if}
{/for}
   

class C{=nodename}BaseDlg : public CMiaDialog
{
	enum Ids
	{
		{for:children("ids")}
			{=@id} = {=@value},
		{/for}
	};

	bool OnEvent(CEvent& e)
	{
		{for:allchildren("event_binding')}
		{if:not parent.hasattrib("__idName")}
		{abort:"Node "+parent.fullpath+" doesn't have an associated ID"}
		{/if}
		if (e.iControlID=={=parent.@__idName} && e.iEventID==C{=outerNode.nodename}::{=@eventName})
			{ {=@methodName}(e); }
		{/for}
	}
}
class {@ClassName} : public {@BaseClass}
{
// Construction
	{@ClassName}()
	{
	}
	virtual ~{@ClassName}()
	{
	}

// IDs
	{for:node("ids").children}
	enum IDs
	{
		{@idname} = {@idvalue};
	};
	{else}
	// None
	{/for}

	BEGIN_EVENT_MAP({@ClassName})
	{with:nodes.Named("eventBinding").first}
	{/with}
	{for:descendants.Named("eventBinding")}
		EVENT({@idname}, C{parent.name}::{@eventtype}, {@handler})
	{/for}
	END_EVENT_MAP()

};



*/

class CResValue
{
public:
	CResValue()
	{
		m_type=rvtEmpty;
	}
	CResValue(bool bValue)
	{
		m_type=rvtBool;
		m_bValue=bValue;
	}
	CResValue(const wchar_t* psz)
	{
		m_type=rvtString;
		m_strValue=psz;
	}
	CResValue(const CUniString& str)
	{
		m_type=rvtString;
		m_strValue=str;
	}
	CResValue(CResNode* pNode)
	{
		m_type=rvtNode;
		m_pNode=pNode;
	}
	CResValue(const CResValue& other)
	{
		Assign(other);
	}
	void AddNode(CResNode* pNode)
	{
		ASSERT(m_type==rvtEmpty || m_type==rvtNodes);
		m_type=rvtNodes;
		m_vecNodes.Add(pNode);
	}

	void Assign(const CResValue& other)
	{
		m_type=other.m_type;
		m_bValue=other.m_bValue;
		m_strValue=other.m_strValue;
		m_pNode=other.m_pNode;
		m_vecNodes.RemoveAll();
		m_vecNodes.Add(m_vecNodes);
	}
	void Clear()
	{
		m_type=rvtEmpty;
		m_strValue.Empty();
		m_vecNodes.RemoveAll();
		m_pNode=NULL;
		m_bValue=false;
	}

	CResValue& operator=(const CResValue& other)
	{
		Assign(other);
		return *this;
	}


	enum Type
	{
		rvtEmpty,
		rvtBool,
		rvtString,
		rvtNodes,
		rvtNode,
	};

	static const wchar_t* TypeName(Type t)
	{
		const wchar_t* pszTypeNames[]={
			L"Empty",
			L"Bool",
			L"String",
			L"Nodes", 
			L"Node",
		};
		return pszTypeNames[t];
	}

	Type		m_type;
	bool		m_bValue;
	CUniString	m_strValue;
	CVector<CResNode*>	m_vecNodes;
	CResNode*	m_pNode;
};

class CResLoopContext
{
public:
	CResLoopContext*	m_pOuterLoop;
	int					m_iIterPos;
	int					m_iIterCount;
};

class CResCodeLocation
{
public:
	CResCodeLocation(const wchar_t* pszLocation) : 
		m_pszLocation(pszLocation)
	{
	}

	const wchar_t* m_pszLocation;
};

class CResContext
{
public:
	// Construction
	CResContext()
	{
		m_pLoopContext=NULL;
		m_pCurrentNode=NULL;
	}

	// Current loop context...
	CResTemplate*		m_pTemplate;
	CResNode*			m_pCurrentNode;
	CResLoopContext*	m_pLoopContext;
	CUniString			m_strOutput;

	// Change a type, generate error if failed
	bool ChangeType(CResCodeLocation* pLocation, CResValue& value, CResValue::Type type)
	{
		switch (type)
		{
			case CResValue::rvtEmpty:
				value.Clear();
				return true;

			case CResValue::rvtBool:
				switch (value.m_type)
				{
					case CResValue::rvtBool:
						break;

					case CResValue::rvtEmpty:
						value=false;
						break;
					
					case CResValue::rvtNode:
						value=bool(value.m_pNode!=NULL);
						break;

					case CResValue::rvtNodes:
						value=!value.m_vecNodes.IsEmpty();
						break;

					case CResValue::rvtString:
						value=!value.m_strValue.IsEmpty();
						break;
				}
				return true;

			case CResValue::rvtString:
				switch (value.m_type)
				{
					case CResValue::rvtBool:
						value=value.m_bValue ? L"true" : L"false";
						break;

					case CResValue::rvtEmpty:
						value="";
						break;
					
					case CResValue::rvtNode:
						value=value.m_pNode ? value.m_pNode->GetName() : L"<null>";
						break;

					case CResValue::rvtNodes:
					{
						CUniString str=L"[";
						for (int i=0; i<value.m_vecNodes.GetSize(); i++)
						{
							if (i!=0)
								str+=L",";
							str+=value.m_vecNodes[i]->GetName();
						}
						str+=L"]";
						value=str;
						break;
					}

					case CResValue::rvtString:
						break;
				}
				return true;

			case CResValue::rvtNode:
				switch (value.m_type)
				{
					case CResValue::rvtNode:
						return true;

					case CResValue::rvtString:
						value=m_pCurrentNode->GetRelativeNode(value.m_strValue);
						return true;
				}
				break;

			case CResValue::rvtNodes:
				switch (value.m_type)
				{
					case CResValue::rvtNodes:
						return true;
				}
				break;
		}

		return SetError(pLocation, Format(L"Can't convert from %s to %s", CResValue::TypeName(value.m_type), CResValue::TypeName(type)));
	}

	bool InError()
	{
		return m_pTemplate->InError();
	}

	bool SetError(CResCodeLocation* pLocation, const wchar_t* pszError)
	{
		m_pTemplate->SetError(pszError, pLocation->m_pszLocation);
		return false;
	}

	void Write(const wchar_t* pszText)
	{
		m_strOutput.Append(pszText);
	}
};

class CResExpression : public CResCodeLocation
{
public:
	CResExpression(const wchar_t* pszLocation) : 
		CResCodeLocation(pszLocation)
	{
	}
	virtual ~CResExpression()
	{
	}

	virtual bool Evaluate(CResContext& ctx, CResValue& Value)=0;
};

class CResExpressionLiteral : public CResExpression
{
public:
	CResExpressionLiteral(const wchar_t* pszLocation, const CResValue& value) : 
		CResExpression(pszLocation),
		m_Value(value)
	{
	}

	virtual ~CResExpressionLiteral()
	{
	}

	virtual bool Evaluate(CResContext& ctx, CResValue& Value)
	{
		Value=m_Value;
		return true;
	}

	CResValue m_Value;
};


class CResExpressionFunction : public CResExpression
{
public:
	CResExpressionFunction(const wchar_t* pszLocation) : 
		CResExpression(pszLocation)
	{
	}

	virtual ~CResExpressionFunction()
	{
	}

	enum Operation
	{
		opAnd,
		opOr,
		opNot,
		opWrite,
	};

	void SetOperation(Operation op)
	{
		m_op=op;
	}

	void AddArg(CResExpression* pArg)
	{
		m_Args.Add(pArg);
	}

	bool EvaluateInternal(CResContext& ctx, CResValue* pArgs, CResValue& Value)
	{
		switch (m_op)
		{
			case opAnd:
				for (int i=0; i<m_Args.GetSize(); i++)
				{
					if (!ctx.ChangeType(this, pArgs[i], CResValue::rvtBool))
						return false;

					if (!pArgs[i].m_bValue)
					{
						Value=false;
						return true;
					}
				}
				Value=true;
				return true;

			case opOr:
				for (int i=0; i<m_Args.GetSize(); i++)
				{
					if (!ctx.ChangeType(this, pArgs[i], CResValue::rvtBool))
						return false;

					if (pArgs[i].m_bValue)
					{
						Value=true;
						return true;
					}
				}
				Value=false;
				return true;

			case opWrite:
				for (int i=0; i<m_Args.GetSize(); i++)
				{
					if (!ctx.ChangeType(this, pArgs[i], CResValue::rvtString))
						return false;

					ctx.Write(pArgs[i].m_strValue);
				}
				return true;

		}

		ASSERT(false);
		return false;
	}

	virtual bool Evaluate(CResContext& ctx, CResValue& Value)
	{
		// Allocate input arg value storage
		CResValue* pValues=m_Args.GetSize() ? new CResValue[m_Args.GetSize()] : NULL;

		// Evaluate all args
		bool bOK=true;
		for (int i=0; i<m_Args.GetSize(); i++)
		{
			if (!m_Args[i]->Evaluate(ctx, pValues[i]))
			{
				bOK=false;
				break;
			}

			// Short circuit and/or logical operations
			if (i==0)
			{
				switch (m_op)
				{
					case opAnd:	
						if (!ctx.ChangeType(this, pValues[i], CResValue::rvtBool))
						{
							delete pValues;
							return false;
						}

						if (!pValues[i].m_bValue)
						{
							Value=false;
							return true;
						}
						break;

					case opOr:
						if (!ctx.ChangeType(this, pValues[i], CResValue::rvtBool))
						{
							delete pValues;
							return false;
						}

						if (pValues[i].m_bValue)
						{
							Value=true;
							return true;
						}
						break;

				}
			}
		}
		
		// Evaluate function
		if (bOK)
		{
			bOK=EvaluateInternal(ctx, pValues, Value);
		}
		
		// Clean up
		if (pValues)
			delete [] pValues;

		// Done
		return bOK;
	}

protected:
	Operation							m_op;
	CVector<CResExpression*, SOwnedPtr>	m_Args;
};

class CResStatement : public CResCodeLocation
{
public:
	CResStatement(const wchar_t* pszLocation) : 
		CResCodeLocation(pszLocation)
	{
	};
	virtual ~CResStatement() 
	{
	};

	virtual bool Execute(CResContext& ctx)=0;
};

class CResStatementText : public CResStatement
{
public:
	CResStatementText(const wchar_t* pszLocation, const CUniString& str) :
		CResStatement(pszLocation),
		m_strText(str)
	{
	}
	virtual ~CResStatementText()
	{
	}

	virtual bool Execute(CResContext& ctx)
	{
		ctx.Write(m_strText);
		return true;
	}

	CUniString m_strText;
};


class CResStatements : public CResStatement
{
public:
	CResStatements(const wchar_t* pszLocation) :
		CResStatement(pszLocation)
	{
	};
	virtual ~CResStatements() 
	{
	};

	virtual bool Execute(CResContext& ctx)
	{
		for (int i=0; i<m_Statements.GetSize(); i++)
		{
			if (!m_Statements[i]->Execute(ctx))
				return false;
		}

		return true;
	}

	void AddStatement(CResStatement* pStatement)
	{
		m_Statements.Add(pStatement);
	}

	void AddText(const CUniString& str)
	{
		if (str.IsEmpty())
			return;

		m_Statements.Add(new CResStatementText(NULL, str));
	}

protected:
	CVector<CResStatement*, SOwnedPtr>	m_Statements;

};

class CResStatementIf : public CResStatement
{
public:
	CResStatementIf(const wchar_t* pszLocation) : 
		CResStatement(pszLocation),
		m_TrueStatements(NULL),
		m_FalseStatements(NULL)
	{
		m_pCondition=NULL;
	}

	virtual ~CResStatementIf()
	{
		if (m_pCondition)
			delete m_pCondition;
	}

	void SetCondition(CResExpression* pCondition)
	{
		m_pCondition=pCondition;
	}

	CResStatements* GetTrueStatements() { return &m_FalseStatements; }
	CResStatements* GetFalseStatements() { return &m_TrueStatements; }


	virtual bool Execute(CResContext& ctx)
	{
		CResValue v;
		if (!m_pCondition->Evaluate(ctx, v) || 
			!ctx.ChangeType(this, v, CResValue::rvtBool))
			return false;

		if (v.m_bValue)
			return m_TrueStatements.Execute(ctx);
		else
			return m_FalseStatements.Execute(ctx);
	}


protected:
	CResExpression*	m_pCondition;
	CResStatements	m_TrueStatements;
	CResStatements	m_FalseStatements;
};

class CResStatementFor : public CResStatementIf
{
public:
	CResStatementFor(const wchar_t* pszLocation) : 
		CResStatementIf(pszLocation)
	{
	}
	virtual ~CResStatementFor() {}

	virtual bool Execute(CResContext& ctx)
	{
		CResValue v;
		if (!m_pCondition->Evaluate(ctx, v) || 
			!ctx.ChangeType(this, v, CResValue::rvtNodes))
			return false;

		if (v.m_vecNodes.GetSize())
		{
			// Setup a loop context
			CResLoopContext loop;
			loop.m_pOuterLoop=ctx.m_pLoopContext;
			loop.m_iIterCount=v.m_vecNodes.GetSize();

			ctx.m_pLoopContext=&loop;
			CResNode* pOldCurrentNode=ctx.m_pCurrentNode;

			// Iterate all nodes
			for (loop.m_iIterPos=0; loop.m_iIterPos<loop.m_iIterCount; loop.m_iIterPos++)
			{
				// Setup current element
				ctx.m_pCurrentNode=v.m_vecNodes[loop.m_iIterPos];

				// Execute all true elements
				if (!m_TrueStatements.Execute(ctx))
				{
					ctx.m_pLoopContext=loop.m_pOuterLoop;
					return false;
				}
			}

			// Restore loop context
			ctx.m_pLoopContext=loop.m_pOuterLoop;
			ctx.m_pCurrentNode=pOldCurrentNode;

			// Done!
			return true;
		}
		else
		{
			return m_FalseStatements.Execute(ctx);
		}
	}
};

class CResStatementWriteExpr : public CResStatement
{
public:
	CResStatementWriteExpr(const wchar_t* pszLocation, CResExpression* pExpression) :
		CResStatement(pszLocation),
		m_pExpression(pExpression)
	{
	}
	virtual ~CResStatementWriteExpr()
	{
		delete m_pExpression;
	}

	virtual bool Execute(CResContext& ctx)
	{
		CResValue v;
		if (!m_pExpression->Evaluate(ctx, v))
			return false;

		if (!ctx.ChangeType(this, v, CResValue::rvtString))
			return false;

		ctx.Write(v.m_strValue);
		return true;
	}

	CResExpression*	m_pExpression;
};

/////////////////////////////////////////////////////////////////////////////
// CResTemplate

// Constructor
CResTemplate::CResTemplate()
{
	m_pStatements=NULL;
}

// Destructor
CResTemplate::~CResTemplate()
{
	if (m_pStatements)
		delete m_pStatements;
}

CResExpression* CResTemplate::ParseExpression(const wchar_t*& pszText)
{
	const wchar_t* p=pszText;
	while (p[0] && p[0]!='}')
		p++;

	if (!p[0])
	{
		SetError(L"Unexpected EOF parsing expression", pszText);
		return NULL;
	}

	CResExpressionFunction* pExpr=new CResExpressionFunction(pszText);
	pExpr->SetOperation(CResExpressionFunction::opWrite);
	pExpr->AddArg(new CResExpressionLiteral(pszText, CUniString(pszText, p-pszText)));

	pszText=p;
	return pExpr;

}

// Skip backwards through line space, returns true if reaches start of line
bool SkipLineSpaceReverse(const wchar_t* pszAllText, const wchar_t*& p)
{
	// Check for just white space before the tag
	while (p>pszAllText && IsLineSpace(p[-1]))
	{
		p--;
	}

	// Start of line found
	return p==pszAllText || p[-1]=='\r' || p[-1]=='\n';
}

bool CResTemplate::IsBlockMode(const wchar_t* pszTagStart, const wchar_t*& pszTagEnd)
{
	// Check nothing significant before the tag
	if (!SkipLineSpaceReverse(m_pszText, pszTagStart))
		return false;

	// Check nothing significant after the tag
	const wchar_t* p=pszTagEnd;
	SkipLineSpace(p);
	if (p[0]!='\0' && !IsEOL(p[0]))
		return false;

	// Skip trailing white space
	SkipEOL(p);
	pszTagEnd=p;
	return true;
}


bool IsOpenTagChar(wchar_t ch)
{
	return ch=='/' || IsIdentifierLeadChar(ch);
}

bool IsBlockOpenTag(const wchar_t* p)
{
	return
			DoesMatch(p, L"{for:") || 
			DoesMatch(p, L"{if:");
}

bool IsBlockCloseTag(const wchar_t* p)
{
	return 	DoesMatch(p, L"{else}") ||
			DoesMatch(p, L"{/");
}



bool CResTemplate::ParseStatements(CResStatements* pStatements, const wchar_t*& pszText, bool bBlockMode)
{
	const wchar_t* p=pszText;
	while (p[0])
	{
		// Store start of this segment
		const wchar_t* pszStartPos=p;

		// Find next open brace
		while (p[0] && 
				!(p[0]=='{' && IsOpenTagChar(p[1])) && 
				!(p[0]=='}' && p[1]=='}')
				)
			p++;

		// EOF?
		if (!p[0])
		{
			pStatements->AddText(CUniString(pszStartPos, p-pszStartPos));
			pszText=p;
			return true;
		}

		// Escaped {{ or }}
		if (p[0]==p[1])
		{
			pStatements->AddText(CUniString(p, 1));
			p+=2;
		}

		ASSERT(p[0]=='{' && p[1]!='}');

		// Append text up to this point.  How much depends on block mode and current tag
		if (IsBlockOpenTag(p) || bBlockMode && IsBlockCloseTag(p))
		{
			// Find start of line before tag
			const wchar_t* pszLineStart=p;
			if (SkipLineSpaceReverse(m_pszText, pszLineStart))
			{
				if (pszStartPos<pszLineStart)
				{
					pStatements->AddText(CUniString(pszStartPos, pszLineStart-pszStartPos));
				}
			}
		}
		else
		{
			pStatements->AddText(CUniString(pszStartPos, p-pszStartPos));
		}

		// Closing tag?
		if (IsBlockCloseTag(p))
		{
			// Yep, end of this block
			pszText=p;
			return true;
		}


		const wchar_t* pszBracePos=p;
		p++;

		// For statement
		if (SkipMatch(p, L"for:"))
		{
			// Create a for statement
			CResStatementFor* pStmt=new CResStatementFor(pszBracePos);
			pStatements->AddStatement(pStmt);
			
			// Parse the expression
			CResExpression* pExpr=ParseExpression(p);
			if (!pExpr)
				return false;
			pStmt->SetCondition(pExpr);

			// Expect closing brace
			if (!SkipMatch(p, L"}"))
			{
				SetError(L"Expected closing brace '}'", p);
				return false;
			}

			// Check if block mode
			bool bBlockMode=IsBlockMode(pszBracePos, p);

			// Parse the loop body
			if (!ParseStatements(pStmt->GetTrueStatements(), p, bBlockMode))
				return false;

			// Parse the else block
			const wchar_t* pszElse=p;
			if (SkipMatch(p, L"{else}"))
			{
				bBlockMode=IsBlockMode(pszElse, p);
				
				if (!ParseStatements(pStmt->GetFalseStatements(), p, bBlockMode))
					return false;
			}

			// End of for statment
			if (!SkipMatch(p, L"{/for}"))
			{
				SetError(L"Expected {/for}", p);
				return false;
			}

			// Skip trailing white space if in block mode
			if (bBlockMode)
				SkipToNextLine(p);

			continue;
		}

		// If statement
		if (SkipMatch(p, L"if:"))
		{
			// Create a for statement
			CResStatementIf* pStmt=new CResStatementIf(pszBracePos);
			pStatements->AddStatement(pStmt);
			
			// Parse the expression
			CResExpression* pExpr=ParseExpression(p);
			if (!pExpr)
				return false;
			pStmt->SetCondition(pExpr);

			// Expect closing brace
			if (!SkipMatch(p, L"}"))
			{
				SetError(L"Expected closing brace '}'", p);
				return false;
			}

			// Check if block mode
			bool bBlockMode=IsBlockMode(pszBracePos, p);

			// Parse the loop body
			if (!ParseStatements(pStmt->GetTrueStatements(), p, bBlockMode))
				return false;

			// Parse the else block
			const wchar_t* pszElse=p;
			if (SkipMatch(p, L"{else}"))
			{
				bBlockMode=IsBlockMode(pszElse, p);
				
				if (!ParseStatements(pStmt->GetFalseStatements(), p, bBlockMode))
					return false;
			}

			// End of for statment
			if (!SkipMatch(p, L"{/if}"))
			{
				SetError(L"Expected {/if}", p);
				return false;
			}

			// Skip trailing white space if in block mode
			if (bBlockMode)
				SkipToNextLine(p);

			continue;
		}

		// Parse an expression
		const wchar_t* pszExprPos=p;
		CResExpression* pExpression=ParseExpression(p);
		if (!pExpression)
			return false;

		// Add a statement to write the expression
		pStatements->AddStatement(new CResStatementWriteExpr(pszExprPos, pExpression));

		// Skip closing brace
		if (!SkipMatch(p, L"}"))
		{
			SetError(L"Expected closing brace '}'", p);
			return false;
		}
	}

	return true;
}

// Parse a template from a string	
bool CResTemplate::ParseString(const CAnyString strContent)
{
	m_strText=strContent.As<wchar_t>();
	m_pszText=m_strText.sz();

	if (!m_pStatements)
	{
		delete m_pStatements;
		m_pStatements=new CResStatements(NULL);
	}

	const wchar_t* p=m_pszText;
	if (!ParseStatements(m_pStatements, p, false))
		return false;

	// Check for EOF
	if (p[0])
	{
		return SetError(L"Unexpected content at end of file", p);
	}

	return true;
}


// Parse a template from a file
bool CResTemplate::ParseFile(const CAnyString& strFileName)
{
	// Load test file
	CUniString str;
	result_t r=LoadText(strFileName, str);
	if (r)
	{
		SetError(Format(L"Failed to load file '%s' - %s", strFileName.As<wchar_t>().sz(), FormatResult(r).sz()), NULL);
		return false;
	}

	return ParseString(str);
}

// Render
bool CResTemplate::Render(CResNode* pNode, CUniString& strOutput)
{
	if (!m_pStatements)
		return false;

	// Reset error
	m_strError.Empty();

	// Setup context
	CResContext ctx;
	ctx.m_pCurrentNode=pNode;
	ctx.m_pTemplate=this;

	if (!m_pStatements->Execute(ctx))
		return false;

	strOutput=ctx.m_strOutput;
	return true;
}

// Get last error
CUniString CResTemplate::GetError()
{
	return m_strError;
}

bool ConvertOffsetToLinePosition(const wchar_t* pszText, const wchar_t* pszPos, int& iLine, int& iPos)
{
	iLine=0;
	const wchar_t* pszLineStart=pszText;
	while (pszText<pszPos)
	{
		if (SkipEOL(pszText))
		{
			iLine++;
			pszLineStart=pszText;
		}
		else
		{
			pszText++;
		}
	}

	iPos=pszPos-pszLineStart;
	return true;
}


bool CResTemplate::SetError(const wchar_t* psz, const wchar_t* pszErrorPos)
{
	if (pszErrorPos)
	{
		int iLine, iPos;
		ConvertOffsetToLinePosition(m_pszText, pszErrorPos, iLine, iPos);

		m_strError=Format(L"Line %i Pos %i - %s", iLine+1, iPos+1, psz);
	}
	else
	{
		m_strError=psz;
	}

	return false;
}

bool CResTemplate::InError()
{
	return !m_strError.IsEmpty();
}

}


