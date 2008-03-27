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
// VariantGrid.h - declaration of CVariantGrid class

#ifndef __VARIANTGRID_H
#define __VARIANTGRID_H

namespace Simple
{

class CVariantVector : public CVector<CComVariant>
{
	typedef CVector<CComVariant> _Base;
public:
	CVariantVector()
	{
	};
	CVariantVector(int iCount)
	{
		InsertAt(0, iCount);
	}

	void InsertAt(int iPos, int iCount)
	{
		for (int i=0; i<iCount; i++)
			{
			CComVariant var;
			_Base::InsertAt(iPos+i, var);
			}
	}
	void RemoveAt(int iPos, int iCount)
	{
		for (int i=0; i<iCount; i++)
			{
			_Base::RemoveAt(iPos);
			}
	}
};

// CVariantGrid Class
class CVariantGrid
{
public:
// Construction
			CVariantGrid();
	virtual ~CVariantGrid();

// Operations
	void Reset();
	void SetWidth(int iWidth);
	void SetHeight(int iHeight);
	void SetSize(int iWidth, int iHeight);
	void EnsureSize(int iWidth, int iHeight);
	void EnableAutoGrow();
	int GetWidth() { return m_iWidth; };
	int GetHeight() { return m_Rows.GetSize(); };
	void InsertRows(int iPos, int iCount);
	void InsertColumns(int iPos, int iCount);
	void DeleteRows(int iPos, int iCount);
	void DeleteColumns(int iPos, int iCount);
	VARIANT& GetCell(int iColumn, int iRow);
	void SetCell(int iColumn, int iRow, VARIANT& var);


// Implementation
protected:
// Attributes
	int			m_iWidth;
	bool		m_bAutoGrow;

// Operations
	CVector<CVariantVector*, SOwnedPtr>	m_Rows;

};

}	// namespace Simple

#endif	// __VARIANTGRID_H

