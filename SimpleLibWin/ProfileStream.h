//////////////////////////////////////////////////////////////////////////
// ProfileStream.h - declaration of CProfileStream class

#ifndef __PROFILESTREAM_H
#define __PROFILESTREAM_H

namespace Simple
{

class CProfileEntry;

HRESULT SIMPLEAPI CreateProfileStream(CProfileEntry* pEntry, IStream** pVal);
HRESULT SIMPLEAPI OpenProfileStream(CProfileEntry* pEntry, IStream** pVal);

}	// namespace Simple

#endif	// __PROFILESTREAM_H

