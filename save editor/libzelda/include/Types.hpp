#ifndef __TYPES_HPP__
#define __TYPES_HPP__

#include <limits.h>

// 8 bits integer types
#if UCHAR_MAX == 0xFF
    typedef signed   char Int8;
    typedef unsigned char Uint8;
#else
    #error No 8 bits integer type for this platform
#endif

// 16 bits integer types
#if USHRT_MAX == 0xFFFF
    typedef signed   short Int16;
    typedef unsigned short Uint16;
#elif UINT_MAX == 0xFFFF
    typedef signed   int Int16;
    typedef unsigned int Uint16;
#elif ULONG_MAX == 0xFFFF
    typedef signed   long Int16;
    typedef unsigned long Uint16;
#else
    #error No 16 bits integer type for this platform
#endif

// 32 bits integer types
#if USHRT_MAX == 0xFFFFFFFF
    typedef signed   short Int32;
    typedef unsigned short Uint32;
#elif UINT_MAX == 0xFFFFFFFF
    typedef signed   int Int32;
    typedef unsigned int Uint32;
#elif ULONG_MAX == 0xFFFFFFFF
    typedef signed   long Int32;
    typedef unsigned long Uint32;
#else
    #error No 32 bits integer type for this platform
#endif

typedef signed   long long Int64;
typedef unsigned long long Uint64;

#endif
