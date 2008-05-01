//////////////////////////////////////////////////////////////////////////
// ProfileFileBinary.h - declaration of ProfileFileBinary

#ifndef __PROFILEFILEBINARY_H
#define __PROFILEFILEBINARY_H

namespace Simple
{

HRESULT SIMPLEAPI SaveBinaryProfile(const CProfileFile& Src, IStream* pStream);
HRESULT SIMPLEAPI LoadBinaryProfile(CProfileFile& Dest, IStream* pStream);

#endif	// __PROFILEFILEBINARY_H

}		// namespace Simple

