//////////////////////////////////////////////////////////////////////////
// ProfileFileBinary.h - declaration of ProfileFileBinary

#ifndef __PROFILEFILEBINARY_H
#define __PROFILEFILEBINARY_H

namespace Simple
{

HRESULT SaveBinaryProfile(const CProfileFile& Src, IStream* pStream);
HRESULT LoadBinaryProfile(CProfileFile& Dest, IStream* pStream);

#endif	// __PROFILEFILEBINARY_H

}		// namespace Simple

