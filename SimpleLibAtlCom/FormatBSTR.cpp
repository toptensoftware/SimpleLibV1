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
// FormatBSTR.cpp - implementation of FormatBSTR

#include "stdafx.h"
#include "SimpleLibAtlComBuild.h"

#include "FormatBSTR.h"

namespace Simple
{

// Creates a string filled with a particular character
void SIMPLEAPI CharString(OLECHAR ch, int iLen, BSTR* pVal)
{
	// Allocate a buffer
	*pVal=::SysAllocStringLen(NULL, iLen);

	// Fill in string
	for (int i=0; i<iLen; i++)
		{
		(*pVal)[i]=ch;
		}
}

// Read a format specifier for FormatBSTR
OLECHAR ReadFormatSpecifier(LPCOLESTR& p, bool* pbLeftAlign, bool* pbShort, bool* pbLong, bool* pbInt64, int* piWidth, int* piPrecision)
{
	// Left/right alignment
	if (p[0]==L'-')
		{
		*pbLeftAlign=true;
		p++;
		}
	else
		*pbLeftAlign=false;

	// Read width
	if (!ReadInt(p, piWidth))
		{
		if (p[0]==L'*')
			{
			*piWidth=-2;
			p++;
			}
		else
			*piWidth=-1;
		}

	// Skip decimal
	if (p[0]==L'.')
		{
		p++;

		// Read precision
		if (!ReadInt(p, piPrecision))
			{
			if (p[0]==L'*')
				{
				*piPrecision=-2;
				p++;
				}
			else
				*piPrecision=-1;
			}
		}

	if (p[0]==L'h')
		{
		*pbShort=true;
		p++;
		}
	else
		{
		*pbShort=false;
		}


	if (p[0]==L'l' || p[0]==L'L')
		{
		*pbLong=true;
		p++;
		}
	else
		{
		*pbLong=false;
		}

	if (p[0]==L'I' && p[1]==L'6' && p[2]==L'4')
		{
		*pbInt64=true;
		p+=3;
		}
	else
		{
		*pbInt64=false;
		}


	OLECHAR ch=p[0];
	p++;
	return ch;
};

// Truncates a string to required precision
void DoStringPrecision(BSTR* pVal, int iPrecision)
{
	int iLen=::SysStringLen(*pVal);
	if (iPrecision>0 && iPrecision<iLen)
		{
		(*pVal)[iPrecision]=L'\0';
		BSTR bstrNew=::SysAllocString(*pVal);
		::SysFreeString(*pVal);
		*pVal=bstrNew;
		}
}

// Pad a number with leading zeros (need to be updated for negatives)
void PadNumber(BSTR* pbstr, int iPrecision)
{
	int iLen=::SysStringLen(*pbstr);
	if (iPrecision <= iLen)
		return;

	// Prepend zeros...
	CComBSTR bstr;
	CharString(L'0', iPrecision-iLen, &bstr);
	bstr.Append(*pbstr);
	::SysFreeString(*pbstr);
	*pbstr=bstr.Detach();
}

// Format a hex number
void SIMPLEAPI FormatHex(DWORD dwValue, int iPrecision, bool bUpperCase, BSTR* pVal)
{
	OLECHAR szBuf[16];
	LPOLESTR p=szBuf;
	int iShift=28;

	while (iShift>=0)
		{
		BYTE bNib=(BYTE)((dwValue>>iShift) & 0xf);
		if (bNib>=0xA)
			*p=(bUpperCase ? L'A' : L'a') + (bNib-0xA);
		else
			*p=L'0' + bNib;

		p++;
		iShift-=4;
		}

	// Null terminate it
	p[0]=L'\0';
	
	// Skip leading zeros...
	p=szBuf;
	while (p[0]==L'0')
		p++;

	// Format number
	*pVal=::SysAllocString(p);

	// Add leading zeros...
	PadNumber(pVal, iPrecision);
}

int SIMPLEAPI FormatBSTR(BSTR* pVal, LPCOLESTR pszFormat, va_list args)
{
	CUniString buf;

	if (pszFormat[0]==L'%' && pszFormat[1]==L'+' && *pVal)
		buf.Assign(*pVal);

	FormatBSTR(buf, pszFormat, args);

	// Return BSTR
	*pVal=SysAllocString(buf);
	return ::SysStringLen(*pVal);
}


void FormatI64(CUniString& buf, __int64 i)
{
	if (i<0)
		{
		buf.Append(L"-");
		i*=-1;
		}

	if (i==0)
		{
		buf.Append(L"0");
		return;
		}
	
	int iPos=buf.GetLength();
	while (i)
		{
		OLECHAR ch='0' + OLECHAR(i%10);
		buf.Insert(iPos, &ch, 1);
		i/=10;
		}
}


void FormatUI64(CUniString& buf, unsigned __int64 i)
{
	if (i==0)
		{
		buf.Append(L"0");
		return;
		}
	
	int iPos=buf.GetLength();
	while (i)
		{
		OLECHAR ch='0' + OLECHAR(i%10);
		buf.Insert(iPos, &ch, 1);
		i/=10;
		}
}


// Format a string
int SIMPLEAPI FormatBSTR(CUniString& buf, LPCOLESTR pszFormat, va_list args)
{
	bool bAppend=pszFormat[0]==L'%' && pszFormat[1]==L'+';
	if (bAppend)
		pszFormat+=2;
	
	if (!bAppend)
		buf.Empty();

	LPCOLESTR p=pszFormat;
	LPCOLESTR pszStart=p;
	#define FLUSH	

	while (p[0])
		{
		if (p[0]==L'%')
			{
			// Flush buffer
			buf.Append(pszStart, int(p-pszStart));

			// Skip the % sign
			p++;

			// Special case for %%
			if (p[0]==L'%')
				{
				buf.Append(p, 1);
				p++;
				}
			else
				{
				// Read format specifier
				int iWidth=-1;
				int iPrecision=-1;
				bool bLeftAlign;
				bool bShort;
				bool bLong;
				bool bInt64;
				CComBSTR bstrValue;
				OLECHAR chSpec=ReadFormatSpecifier(p, &bLeftAlign, &bShort, &bLong, &bInt64, &iWidth, &iPrecision);

				// If width or precision use passed in arg, read it
				if (iWidth==-2) 	iWidth=va_arg(args, int);
				if (iPrecision==-2)	iPrecision=va_arg(args, int);

				// Handle short/long overrides
				if (chSpec=='C' && bLong)								
					chSpec='c';
				if (chSpec=='c' && bShort)
					chSpec='C';
				if (chSpec=='S' && bLong)
					chSpec='s';
				if (chSpec=='s' && bShort)
					chSpec='S';

				// Process it
				switch (chSpec)
					{
					case L'c':
						bstrValue=CComBSTR(1, &va_arg(args, OLECHAR));
						break;

					case L'C':
						{
						OLECHAR ch;
						MultiByteToWideChar(CP_ACP, 0, &va_arg(args, char), 1, &ch, 1);
						bstrValue=CComBSTR(1, &ch);
						}
						break;

					case L's':
						bstrValue=va_arg(args, LPCOLESTR);
						DoStringPrecision(&bstrValue, iPrecision);
						break;

					case L'S':
						bstrValue=va_arg(args, LPCSTR);
						DoStringPrecision(&bstrValue, iPrecision);
						break;

					case L'u':
						if (bInt64)
							{
							FormatUI64(buf, va_arg(args, unsigned __int64));
							}
						else
							{
							if (bShort)
								VarBstrFromUI2(va_arg(args, unsigned short), LOCALE_USER_DEFAULT, 0, &bstrValue);
							else
								VarBstrFromUI4(va_arg(args, unsigned int), LOCALE_USER_DEFAULT, 0, &bstrValue);
							}
						PadNumber(&bstrValue, iPrecision);
						break;

					case L'i':
					case L'd':
						if (bInt64)
							{
							FormatI64(buf, va_arg(args, __int64));
							}
						else
							{
							if (bShort)
								VarBstrFromI2(va_arg(args, short), LOCALE_USER_DEFAULT, 0, &bstrValue);
							else
								VarBstrFromI4(va_arg(args, int), LOCALE_USER_DEFAULT, 0, &bstrValue);
							}
						PadNumber(&bstrValue, iPrecision);
						break;

					case L'f':
						{
						if (bShort)
							{
							VarBstrFromR4(va_arg(args, float), LOCALE_USER_DEFAULT, 0, &bstrValue);
							}
						else
							{
							VarBstrFromR8(va_arg(args, double), LOCALE_USER_DEFAULT, 0, &bstrValue);
							}
						}
						break;

					case L'g':
						{
						if (bShort)
							{
							CComVariant varNum(va_arg(args, float));
							VarFormatNumber(&varNum, iPrecision, -2, -2, -2, 0, &bstrValue);
							}
						else
							{
							CComVariant varNum(va_arg(args, double));
							VarFormatNumber(&varNum, iPrecision, -2, -2, -2, 0, &bstrValue);
							}
						}
						break;

					case L'v':
						{
						CComVariant varTemp;
						if (FAILED(varTemp.ChangeType(VT_BSTR, &va_arg(args, VARIANT))))
							varTemp=L"<err>";
						bstrValue.Attach(V_BSTR(&varTemp));
						varTemp.vt=VT_EMPTY;
						}
						break;

					case L'V':
						{
						VARIANT var=va_arg(args, VARIANT);
						LPOLESTR pszFormat=va_arg(args, LPOLESTR);
						if (FAILED(VarFormat(&var, pszFormat, 0, 0, 0, &bstrValue)))
							bstrValue=L"<err>";
						}
						break;

					case L'x':
					case L'X':
						if (bShort)
							FormatHex(va_arg(args, WORD), iPrecision, chSpec==L'X', &bstrValue);
						else
							FormatHex(va_arg(args, DWORD), iPrecision, chSpec==L'X', &bstrValue);
						break;

					case L'y':
					case L'Y':
						{
						CComVariant varNum(va_arg(args, double));
						V_VT(&varNum)=VT_CY;
						VarFormatCurrency(&varNum, iPrecision, -2, -2, -2, 0, &bstrValue);
						}
						break;

					case L'p':
					case L'P':
						{
						CComVariant varNum(va_arg(args, double));
						VarFormatPercent(&varNum, iPrecision, -2, -2, -2, 0, &bstrValue);
						}
						break;

					case L'j':
						{
						CComVariant varNum(va_arg(args, double));
						VarFormatDateTime(&varNum, 2, 0, &bstrValue);
						}
						break;

					case L'J':
						{
						CComVariant varNum(va_arg(args, double));
						VarFormatDateTime(&varNum, 1, 0, &bstrValue);
						}
						break;

					case L't':
						{
						CComVariant varNum(va_arg(args, double));
						VarFormatDateTime(&varNum, 4, 0, &bstrValue);
						}
						break;

					case L'T':
						{
						CComVariant varNum(va_arg(args, double));
						VarFormatDateTime(&varNum, 3, 0, &bstrValue);
						}
						break;

					case L'E':
						bstrValue=FormatError(va_arg(args, HRESULT));
						break;

					case L'z':
						{
						int i=va_arg(args, int);
						}
						break;
					}

				// Ensure have something to work with
				if (!bstrValue.m_str)
					bstrValue=L"";

				// Setup width
				if (iWidth>(int)bstrValue.Length())
					{
					// Create padding string
					CComBSTR bstrPad;
					CharString(L' ', iWidth-bstrValue.Length(), &bstrPad);

					if (bLeftAlign)
						{
						// Left align
						bstrValue.Append(bstrPad);
						}
					else
						{
						// Right align
						bstrPad.Append(bstrValue);
						bstrValue.Empty();
						bstrValue.Attach(bstrPad.Detach());
						}
					}

				// Append the value
				buf.Append(bstrValue, bstrValue.Length());
				}

			// Store start of next segment
			pszStart=p;
			}
		else
			{
			// Next character
			p++;
			}
		}

	// Flush remaining chars...
	buf.Append(pszStart, int(p-pszStart));

	return int(buf.GetLength());
}

int SIMPLEAPI FormatBSTR(BSTR* pVal, LPCOLESTR pszFormat, ...)
{
	va_list args;
	va_start(args, pszFormat);
	int iRet=FormatBSTR(pVal, pszFormat, args);
	va_end(args);
	return iRet;
}


CComBSTR SIMPLEAPI FormatBSTR(LPCOLESTR pszFormat, ...)
{
	va_list args;
	va_start(args, pszFormat);
	CComBSTR bstr;
	FormatBSTR(&bstr, pszFormat, args);
	va_end(args);
	return bstr;
}

int SIMPLEAPI FormatBSTR(BSTR* pVal, UINT nResIDFormat, va_list args)
{
	CComBSTR b;
	b.LoadString(nResIDFormat);
	return FormatBSTR(pVal, b, args);
}

int SIMPLEAPI FormatBSTR(BSTR* pVal, UINT nResIDFormat, ...)
{
	va_list args;
	va_start(args, nResIDFormat);
	int iRetv=FormatBSTR(pVal, nResIDFormat, args);
	va_end(args);
	return iRetv;
}

CComBSTR SIMPLEAPI FormatBSTR(UINT nResIDFormat, ...)
{
	va_list args;
	va_start(args, nResIDFormat);
	CComBSTR bstr;
	FormatBSTR(&bstr, nResIDFormat, args);
	va_end(args);
	return bstr;
}


}	// namespace Simple
