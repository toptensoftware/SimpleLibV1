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
// SpecifyPropertyPagesImpl2.h - declaration of ISpecifyPropertyPagesImpl2

#ifndef __SPECIFYPROPERTYPAGESIMPL2_H
#define __SPECIFYPROPERTYPAGESIMPL2_H

#ifdef __ATLCOM_H__

namespace Simple
{

/////////////////////////////////////////////////////////////////////////////
// Support for design mode only property pages...

#define PROP_PAGE_DESIGNMODE(clsid) \
		{NULL, NULL, &clsid, &clsid, 0, 0, 0},


template <class T>
class ATL_NO_VTABLE ISpecifyPropertyPagesImpl2 : public ISpecifyPropertyPages
{
public:
	// ISpecifyPropertyPages
	//
	STDMETHOD(GetPages)(CAUUID* pPages)
	{
		ATLTRACE2(atlTraceCOM, 0, _T("ISpecifyPropertyPagesImpl::GetPages\n"));
		ATL_PROPMAP_ENTRY* pMap = T::GetPropertyMap();
		T* pThis=static_cast<T*>(this);
		BOOL bUserMode;
		pThis->GetAmbientUserMode(bUserMode);
		return GetPagesHelper(pPages, pMap, !bUserMode);
	}
protected:
	HRESULT GetPagesHelper(CAUUID* pPages, ATL_PROPMAP_ENTRY* pMap, bool bDesignMode)
	{
		ATLASSERT(pMap != NULL);
		if (pMap == NULL)
			return E_POINTER;

		int nCnt = 0;
		// Get count of unique pages to alloc the array
		int i;
		for (i = 0; pMap[i].pclsidPropPage != NULL; i++)
		{
			// only allow non data entry types
			if (pMap[i].vt == 0)
			{
				// Does this property have a page?  CLSID_NULL means it does not
				if (!(*pMap[i].pclsidPropPage==CLSID_NULL))
					nCnt++;
			}
		}
		pPages->pElems = (GUID*) CoTaskMemAlloc(sizeof(CLSID)*nCnt);
		if (pPages->pElems == NULL)
			return E_OUTOFMEMORY;
		// reset count of items we have added to the array
		nCnt = 0;
		for (i = 0; pMap[i].pclsidPropPage != NULL; i++)
		{
			// only allow non data entry types
			if (pMap[i].vt == 0)
			{
				// Does this property have a page?  CLSID_NULL means it does not
				if (!(*pMap[i].pclsidPropPage==CLSID_NULL))
				{
					BOOL bFound = FALSE;
					// Search through array we are building up to see
					// if it is already in there
					for (int j=0; j<nCnt; j++)
					{
						if (*pMap[i].pclsidPropPage==pPages->pElems[j])
						{
							// It's already there, so no need to add it again
							bFound = TRUE;
							break;
						}
					}
					// If we didn't find it in there then add it
					if (!bFound)
						pPages->pElems[nCnt++] = *pMap[i].pclsidPropPage;
				}
			}
		}
		
		// Now remove designmode only pages...
		if (!bDesignMode)
			{
			for (i=0; pMap[i].pclsidPropPage!=NULL; i++)
				{
				if (pMap[i].pclsidPropPage==pMap[i].piidDispatch)
					{
					for (int j=0; j<nCnt; j++)
						{
						if (*pMap[i].pclsidPropPage==pPages->pElems[j])
							{
							// Remove it...
							if (j<nCnt-1)
								{
								memcpy(&pPages->pElems[j], &pPages->pElems[j+1], (nCnt-j-1)*sizeof(GUID));
								}
							nCnt--;
							j--;
							}
						}
					}
				}
			}

		pPages->cElems = nCnt;
		return S_OK;
	}

};

}	// namespace Simple

#endif	// __ATLCOM_H__
#endif	// __SPECIFYPROPERTYPAGESIMPL2_H