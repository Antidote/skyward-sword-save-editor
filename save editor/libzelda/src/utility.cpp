#include "utility.hpp"
#include <iostream>
#include <string.h>

bool isEmpty(Int8* buf, size_t size)
{
    return buf[0] == 0 && !memcmp(buf, buf + 1, size - 1);
}

unsigned short swapU16(unsigned short val )
{
    return (val << 8) | (val >> 8 );
}

short swap16(short val )
{
    return (val << 8) | ((val >> 8) & 0xFF);
}

unsigned int swapU32(unsigned int val)
{
    val = (val & 0x0000FFFF) << 16 | (val & 0xFFFF0000) >> 16;
    val = (val & 0x00FF00FF) << 8 | (val & 0xFF00FF00) >> 8;
    return (Uint32)val;
}

int swap32( int val )
{
    val = (val & 0x0000FFFF) << 16 | (val & 0xFFFF0000) >> 16;
    val = (val & 0x00FF00FF) << 8 | (val & 0xFF00FF00) >> 8;
    return val;
}

long long swap64(long long val)
{
    return ((long long)((((long long)(val) & 0xFF00000000000000ULL) >> 56) |
          (((long long)(val) & 0x00FF000000000000ULL) >> 40) |
          (((long long)(val) & 0x0000FF0000000000ULL) >> 24) |
          (((long long)(val) & 0x000000FF00000000ULL) >>  8) |
          (((long long)(val) & 0x00000000FF000000ULL) <<  8) |
          (((long long)(val) & 0x0000000000FF0000ULL) << 24) |
          (((long long)(val) & 0x000000000000FF00ULL) << 40) |
          (((long long)(val) & 0x00000000000000FFULL) << 56)));
}

bool isSystemBigEndian()
{
    char* test = (char*)"\xFE\xFF";
    return (*(unsigned short*)test == 0xFEFF);
}
