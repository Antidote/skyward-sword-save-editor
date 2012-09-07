#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <string>
#include "tools.h"

bool isEmpty(u8*, size_t);

unsigned short swapU16(unsigned short val );
         short swap16 (short val );
unsigned int   swapU32(unsigned int val);
         int   swap32 (int val );
    long long  swap64 (long long val);

void sillyRandom(u8 * rndArea, u8 count);

u8* getSha1(u8* stuff, u32 stuffSize);

bool isSystemBigEndian();

#endif
