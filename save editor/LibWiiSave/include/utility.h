#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <string>
#include <Types.hpp>

#ifdef __cplusplus
extern "C" {
#endif

bool isEmpty(Uint8*, size_t);

Uint16 swapU16(Uint16 val);
 Int16 swap16 ( Int16 val);
Uint32 swapU32(Uint32 val);
 Int32 swap32 ( Int32 val);
 Int64 swap64 ( Int64 val);

void sillyRandom(Uint8 * rndArea, Uint8 count);

Uint8* getSha1(Uint8* stuff, Uint32 stuffSize);

bool isSystemBigEndian();

#ifdef __cplusplus
}
#endif

#endif
