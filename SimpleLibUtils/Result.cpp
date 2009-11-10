//////////////////////////////////////////////////////////////////////////
// Result.cpp - implementation of result_t

#include "StdAfx.h"
#include "SimpleLibUtilsBuild.h"

#include "Result.h"
#include "StringReplace.h"

namespace Simple
{

CUniString FormatResult(result_t r)
{
	switch (RESULT_TYPE(r))
	{
		case RESULT_TYPE_ERRNO:
		{
			CUniString str;
#ifdef _MSC_VER
			_wcserror_s(str.GetBuffer(1024), 1024, RESULT_CODE(r));
#else
			CAnsiString strTemp;
			str=a2w(strerror_r(RESULT_CODE(r), strTemp.GetBuffer(1024), 1024));
#endif
			str=StringReplace(str, L"\r\n", L"", false);
			str=StringReplace(str, L"\n", L"", false);
		}

		case RESULT_TYPE_GENERIC:
		{
			switch (r)
			{
				case s_ok: return L"No error";
				case e_notimpl:	return L"Not implemented";
				case e_fail: return L"Unspecified error";
				case e_invalidarg: return L"Invalid argument";
				case e_accessdenied: return L"Access denied";
				case e_outofmemory: return L"Out of memory";
				case e_fileformat: return L"File format is incorrect or corrupt";
				case e_eof: return L"File format is incorrect or corrupt (unexpected eof)";
			}
		}

	}

	return Format(L"Unknown error: 0x%.8x", r);

}

}
