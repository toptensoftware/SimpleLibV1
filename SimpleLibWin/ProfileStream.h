//////////////////////////////////////////////////////////////////////////
// ProfileStream.h - declaration of CProfileStream class

#ifndef __PROFILESTREAM_H
#define __PROFILESTREAM_H

namespace Simple
{

class CProfileEntry;

HRESULT CreateProfileStream(CProfileEntry* pEntry, IStream** pVal);
HRESULT OpenProfileStream(CProfileEntry* pEntry, IStream** pVal);

}	// namespace Simple

#endif	// __PROFILESTREAM_H

