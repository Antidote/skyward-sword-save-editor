#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <string>
#include "Types.hpp"

bool isEmpty(Int8*, size_t);

unsigned short swapU16(unsigned short val );
         short swap16 (short val );
unsigned int   swapU32(unsigned int val);
         int   swap32 (int val );
    long long  swap64 (long long val);

bool isSystemBigEndian();

#endif
