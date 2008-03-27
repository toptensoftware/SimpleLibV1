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
// AreFontsEqual.cpp - implementation of AreFontsEqual

#include "stdafx.h"
#include "SimpleLibAtlComBuild.h"

#include "VariantGrid.h"

namespace Simple
{

//////////////////////////////////////////////////////////////////////////
// CVariantGrid

// Constructor
CVariantGrid::CVariantGrid()
{
	m_bAutoGrow=false;
	Reset();
}

// Destructor
CVariantGrid::~CVariantGrid()
{
}

void CVariantGrid::Reset()
{
	m_iWidth=0;
	m_Rows.RemoveAll();
}

void CVariantGrid::SetWidth(int iWidth)
{
	ASSERT(iWidth>=0);

	if (m_iWidth==iWidth)
		return;

	if (iWidth>m_iWidth)
		InsertColumns(-1, iWidth-m_iWidth);
	else
		DeleteColumns(iWidth, m_iWidth-iWidth);
}

void CVariantGrid::SetHeight(int iHeight)
{
	ASSERT(iHeight>=0);
	if (m_Rows.GetSize()==iHeight)
		return;
	if (iHeight>m_Rows.GetSize())
		InsertRows(-1, iHeight-m_Rows.GetSize());
	else
		DeleteRows(iHeight, m_Rows.GetSize()-iHeight);
}

void CVariantGrid::SetSize(int iWidth, int iHeight)
{
	SetWidth(iWidth);
	SetHeight(iHeight);
}

void CVariantGrid::EnsureSize(int iWidth, int iHeight)
{
	if (iWidth>GetWidth())
		SetWidth(iWidth);
	if (iHeight>GetHeight())
		SetHeight(iHeight);
}

void CVariantGrid::EnableAutoGrow()
{
	m_bAutoGrow=true;
}


VARIANT& CVariantGrid::GetCell(int iColumn, int iRow)
{
	if (m_bAutoGrow)
		{
		EnsureSize(iColumn+1, iRow+1);
		}
	else
		{
		ASSERT(iColumn>=0 && iColumn<m_iWidth);
		ASSERT(iRow>=0 && iRow<m_Rows.GetSize());
		}
	return m_Rows[iRow]->GetAt(iColumn);
}

void CVariantGrid::SetCell(int iColumn, int iRow, VARIANT& var)
{
	VariantCopy(&GetCell(iColumn, iRow), &var);
}

void CVariantGrid::InsertRows(int iPos, int iCount)
{
	ASSERT(iCount>=0);

	// Quit if nothing
	if (iCount==0)
		return;

	// Work out position
	if (iPos<0 || iPos>m_Rows.GetSize()) iPos=m_Rows.GetSize();

	// Insert the rows...
	for (int i=0; i<iCount; i++)
		{
		m_Rows.InsertAt(iPos+i, new CVariantVector(m_iWidth));
		}

}

void CVariantGrid::InsertColumns(int iPos, int iCount)
{
	ASSERT(iCount>=0);

	// Quit if nothing
	if (iCount==0)
		return;

	// Work out position
	if (iPos<0 || iPos>m_iWidth) iPos=m_iWidth;

	// Insert the columns
	for (int i=0; i<m_Rows.GetSize(); i++)
		{
		m_Rows[i]->InsertAt(iPos, iCount);
		}

	m_iWidth+=iCount;
}

void CVariantGrid::DeleteRows(int iPos, int iCount)
{
	ASSERT(iCount>=0);

	if (iCount==0)
		return;

	ASSERT(iPos>=0);
	ASSERT(iPos+iCount<=m_Rows.GetSize());

	for (int i=0; i<iCount; i++)
		{
		m_Rows.RemoveAt(iPos);
		}
}

void CVariantGrid::DeleteColumns(int iPos, int iCount)
{
	ASSERT(iCount>=0);

	if (iCount==0)
		return;

	ASSERT(iPos>=0);
	ASSERT(iPos+iCount<=m_iWidth);

	for (int i=0; i<m_Rows.GetSize(); i++)
		{
		m_Rows[i]->RemoveAt(iPos, iCount);
		}

	m_iWidth=m_iWidth-iCount;
}


}	// namespace Simple
