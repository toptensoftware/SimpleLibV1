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
#include "SimpleLibUtilsBuild.h"
#include "LoadSaveTextFile.h"

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
	CResNode*			m_pCurrentElement;
};

class CResContext
{
public:
	// Construction
	CResContext()
	{
		m_pLoopContext=NULL;
	}

	// Current loop context...
	CResLoopContext*	m_pLoopContext;

	// Change a type, generate error if failed
	bool ChangeType(CResValue& value, CResValue::Type type);
};

class CResExpression
{
public:
	CResExpression()
	{
	}
	virtual ~CResExpression()
	{
	}

	virtual bool Evaluate(CResContext& ctx, CResValue& Value)=0;
};

class CResExpressionFunction : public CResExpression
{
public:
	CResExpressionFunction()
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

class CResStatement
{
public:
			CResStatement() {};
	virtual ~CResStatement() {};

	virtual bool Execute(CResContext& ctx)=0;
};

class CResStatements : public CResStatement
{
public:
			CResStatements() {};
	virtual ~CResStatements() {};

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

protected:
	CVector<CResStatement*, SOwnedPtr>	m_Statements;

};

class CResStatementIf : public CResStatement
{
public:
	CResStatementIf()
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
			!ctx.ChangeType(v, CResValue::rvtBool))
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
	CResStatementFor() {}
	virtual ~CResStatementFor() {}

	virtual bool Execute(CResContext& ctx)
	{
		CResValue v;
		if (!m_pCondition->Evaluate(ctx, v) || 
			!ctx.ChangeType(v, CResValue::rvtNodes))
			return false;

		if (v.m_vecNodes.GetSize())
		{
			// Setup a loop context
			CResLoopContext loop;
			loop.m_pOuterLoop=ctx.m_pLoopContext;
			loop.m_iIterCount=v.m_vecNodes.GetSize();

			// Iterate all nodes
			for (loop.m_iIterPos=0; loop.m_iIterPos<loop.m_iIterCount; loop.m_iIterPos++)
			{
				// Setup current element
				loop.m_pCurrentElement=v.m_vecNodes[loop.m_iIterPos];

				// Execute all true elements
				if (!m_TrueStatements.Execute(ctx))
				{
					ctx.m_pLoopContext=loop.m_pOuterLoop;
					return false;
				}
			}

			// Restore loop context
			ctx.m_pLoopContext=loop.m_pOuterLoop;

			// Done!
			return true;
		}
		else
		{
			return m_FalseStatements.Execute(ctx);
		}
	}
};


/////////////////////////////////////////////////////////////////////////////
// CResTemplate

// Constructor
CResTemplate::CResTemplate()
{
	m_pRootStatement=NULL;
}

// Destructor
CResTemplate::~CResTemplate()
{
	if (m_pRootStatement)
		delete m_pRootStatement;
}

// Parse a template from a string	
bool CResTemplate::ParseString(const CAnyString strContent)
{
	if (m_pRootStatement)
	{
		delete m_pRootStatement;
		m_pRootStatement=new CResStatements();
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
		SetError(Format(L"Failed to load file '%s' - %s", strFileName.As<wchar_t>().sz(), FormatResult(r).sz()));
		return false;
	}

	return ParseString(str);
}

// Render
CUniString CResTemplate::Render(CResNode* pNode)
{
	return L"";
}

// Get last error
CUniString CResTemplate::GetError()
{
	return m_strError;
}

void CResTemplate::SetError(const wchar_t* psz)
{
	m_strError=psz;
}

}
