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

/////////////////////////////////////////////////////////////////////////////
// IOleCommandTargetImpl

#ifndef _OLECOMMANDTARGETIMPL_H
#define _OLECOMMANDTARGETIMPL_H

#include <docobj.h>


namespace Simple
{

/////////////////////////////////////////////////////////////////////////////
// IOleCommandTargetImpl

template <class T>
class ATL_NO_VTABLE IOleCommandTargetImpl : public IOleCommandTarget
{
public:
	STDMETHODIMP QueryStatus(const GUID* pguidCmdGroup, ULONG cCmds, OLECMD prgCmds[], OLECMDTEXT* pCmdText)
	{
		T* pThis=static_cast<T*>(this);
		for (ULONG i=0; i<cCmds; i++)
			{
			HRESULT hr=pThis->ProcessOleCommand(true, pguidCmdGroup, prgCmds[i].cmdID, 0, NULL, NULL, prgCmds+i, pCmdText ? pCmdText+i : NULL);

			// Stop processing if unknown group
			if (hr==OLECMDERR_E_UNKNOWNGROUP)
				return OLECMDERR_E_UNKNOWNGROUP;

			if (FAILED(hr))
				{
				if (pCmdText && pCmdText->cwBuf)
					{
					pCmdText->rgwz[0]=L'\0';
					pCmdText->cwActual=0;
					}
				}
			}

		return S_OK;
	};
	STDMETHODIMP Exec(const GUID* pguidCmdGroup, DWORD nCmdID, DWORD nCmdexecopt, VARIANT* pvaIn, VARIANT* pvaOut)
	{
		T* pThis=static_cast<T*>(this);
		return pThis->ProcessOleCommand(false, pguidCmdGroup, nCmdID, nCmdexecopt, pvaIn, pvaOut, NULL, NULL);
	};

	bool IsCommandEnabled(DWORD nCmdID, const GUID* pguidCmdGroup=NULL)
	{
		OLECMD cmd;
		cmd.cmdID=nCmdID;
		cmd.cmdf=0;
		if (FAILED(QueryStatus(pguidCmdGroup, 1, &cmd, NULL)))
			return false;

		return (cmd.cmdf & (OLECMDF_SUPPORTED|OLECMDF_ENABLED))==(OLECMDF_SUPPORTED|OLECMDF_ENABLED);
	}

	// Helper to execute a command
	HRESULT ExecuteCommand(DWORD nCmdID, const GUID* pguidCmdGroup=NULL, DWORD nCmdexecopt=OLECMDEXECOPT_DODEFAULT)
	{
		return Exec(pguidCmdGroup, nCmdID, nCmdexecopt, NULL, NULL);
	}

public:
};


/////////////////////////////////////////////////////////////////////////////
// Macro city...



#define BEGIN_COMMANDTARGET_MAP(c) \
	HRESULT ProcessOleCommand(bool bQuery, const GUID* pguidCmdGroup, DWORD nCmdID, DWORD nCmdexecopt, VARIANT* pvarIn, VARIANT* pvarOut, OLECMD* pCmd, OLECMDTEXT* pCmdText) \
	{


#define BEGIN_COMMANDTARGET_GROUP(pguid) \
		{ \
		const GUID* pg=(pguid); \
		if ((pg==NULL && pguidCmdGroup==NULL) || (pg!=NULL && pguidCmdGroup!=NULL && (*pg)==(*pguidCmdGroup))) \
			{ \
			bool bQueryHandled=false;


#define COMMANDTARGET_HANDLER(id, exechandler) \
			if (nCmdID==(id)) \
				{ \
				if (bQuery) \
					{ \
					if (!bQueryHandled) \
						{ \
						pCmd->cmdf=OLECMDF_SUPPORTED|OLECMDF_ENABLED; \
						bQueryHandled=true; \
						} \
					} \
				else \
					{ \
					if (!IsCommandEnabled((id), pguidCmdGroup)) \
						return OLECMDERR_E_DISABLED; \
					return exechandler(nCmdID, nCmdexecopt); \
					} \
				}

#define COMMANDTARGET_RANGE_HANDLER(idFirst, idLast, exechandler) \
			if (nCmdID>=(idFirst) && nCmdID<=(idLast)) \
				{ \
				if (bQuery) \
					{ \
					if (!bQueryHandled) \
						{ \
						pCmd->cmdf=OLECMDF_SUPPORTED|OLECMDF_ENABLED; \
						bQueryHandled=true; \
						} \
					} \
				else \
					{ \
					if (!IsCommandEnabled(nCmdID, pguidCmdGroup)) \
						return OLECMDERR_E_DISABLED; \
					return exechandler(nCmdID, nCmdexecopt); \
					} \
				}

#define COMMANDTARGET_HANDLER_EX(id, exechandler) \
			if (nCmdID==(id)) \
				{ \
				if (bQuery) \
					{ \
					if (!bQueryHandled) \
						{ \
						pCmd->cmdf=OLECMDF_SUPPORTED|OLECMDF_ENABLED; \
						bQueryHandled=true; \
						} \
					} \
				else \
					{ \
					if (!IsCommandEnabled((id), pguidCmdGroup)) \
						return OLECMDERR_E_DISABLED; \
					return exechandler(nCmdID, nCmdexecopt, pvarIn, pvarOut); \
					} \
				}

#define COMMANDTARGET_QUERYHANDLER(id, queryhandler) \
			if (nCmdID==(id) && bQuery)	 \
				{ \
				queryhandler(pCmd, pCmdText); \
				bQueryHandled=true; \
				}

#define COMMANDTARGET_RANGE_QUERYHANDLER(idFirst, idLast, queryhandler) \
			if (nCmdID>=(idFirst) && nCmdID<=(idLast) && bQuery) \
				{ \
				queryhandler(pCmd, pCmdText); \
				bQueryHandled=true; \
				}

// Forward command to another IOleCommandTarget
#define COMMANDTARGET_FORWARD(id, target) \
			if (nCmdID==(id)) \
				{ \
				if (bQuery) \
					{ \
					return CComQIPtr<IOleCommandTarget>(target)->QueryStatus(pguidCmdGroup, 1, pCmd, pCmdText); \
					} \
				else \
					{ \
					return CComQIPtr<IOleCommandTarget>(target)->Exec(pguidCmdGroup, nCmdID, nCmdexecopt, pvarIn, pvarOut); \
					} \
				}

// Forward a command to another IOleCommandTarget as a different command ID/group
#define COMMANDTARGET_FORWARD_AS(id, target, pguidNewGroup, newID) \
			if (nCmdID==(id)) \
				{ \
				if (bQuery) \
					{ \
					pCmd->cmdID=newID; \
					HRESULT hr=CComQIPtr<IOleCommandTarget>(target)->QueryStatus(pguidNewGroup, 1, pCmd, pCmdText); \
					pCmd->cmdID=id; \
					return hr; \
					} \
				else \
					{ \
					return CComQIPtr<IOleCommandTarget>(target)->Exec(pguidNewGroup, newID, nCmdexecopt, pvarIn, pvarOut); \
					} \
				}
/* 
NOT TESTED
#define COMMANDTARGET_NAME(id, pszName) \
			if (nCmdID==(id) && bQuery && pCmdText && pCmdText->cmdtextf==OLECMDTEXTF_NAME) \
				{ \
				wcsncpy(pCmdText->rgwz, pszName, pCmdText->cwBuf-1);	\
				pCmdText->rgwz[pCmdText->cwBuf-1]=L'\0'; \
				pCmdText->cwActual=wcslen(pCmdText->rgwz); \
				}
*/

#define END_COMMANDTARGET_GROUP() \
			if (bQuery) \
				{ \
				if (!bQueryHandled) \
					pCmd->cmdf=0; \
				return S_OK; \
				} \
			else \
				{ \
				return OLECMDERR_E_NOTSUPPORTED; \
				} \
			} \
		}


#define END_COMMANDTARGET_MAP()	\
		return OLECMDERR_E_UNKNOWNGROUP; \
	};


// Put this in your message map to have WM_COMMAND messages dispatched to the
//	IOleCommandTarget implementation
#define COMMAND_TARGET_HANDLER(pGuidCmds) \
	if(uMsg == WM_COMMAND) \
	{ \
		UINT nCmd=LOWORD(wParam); \
		if (Exec(pGuidCmds, nCmd, OLECMDEXECOPT_DODEFAULT, NULL, NULL)==S_OK) \
			{ \
			bHandled=TRUE; \
			return 0; \
			} \
	}


}	// namespace Simple

#endif		// _OLECOMMANDTARGETIMPL_H

