//////////////////////////////////////////////////////////////////////////
// Platform.h - declaration of Platform

#ifndef __PLATFORM_H
#define __PLATFORM_H

#ifndef _WIN32
#define _POSIX
#endif

#if defined(_MSC_VER) && (_MSC_VER >= 1400)
#include <intrin.h>
#ifdef _M_IX86
#pragma intrinsic(_InterlockedCompareExchange)
#pragma warning(disable:4311)
#pragma warning(disable:4312)
inline void* _InterlockedCompareExchangePointer(void** volatile pDest, void* pExchange, void* pCompare)
{
	return (void*)_InterlockedCompareExchange((long*)pDest, (long)pExchange, (long)pCompare);
}
#pragma warning(default:4311)
#pragma warning(default:4312)
#else
#include <intrin.h>
#pragma intrinsic(_InterlockedCompareExchange64)
inline void* _InterlockedCompareExchangePointer(void** volatile pDest, void* pExchange, void* pCompare)
{
	return (void*)_InterlockedCompareExchange64((__int64*)pDest, (__int64)pExchange, (__int64)pCompare);
}
#endif
#else
#define _SIMPLELIB_NO_COMPAREANDEXCHANGE
#endif










#endif	// __PLATFORM_H

