//////////////////////////////////////////////////////////////////////////
// RegStream.h - declaration of CRegStream class

#ifndef __REGSTREAM_H
#define __REGSTREAM_H

namespace Simple
{



// Create/open a stream on a registry key
HRESULT CreateRegistryStream(HKEY hKey, const wchar_t* pszSubKey, const wchar_t* pszValueName, IStream** pVal);
HRESULT OpenRegistryStream(HKEY hKey, const wchar_t* pszSubKey, const wchar_t* pszValueName, IStream** pVal);

}	// namespace Simple

#endif	// __REGSTREAM_H

