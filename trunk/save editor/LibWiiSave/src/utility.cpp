#include "utility.h"
#include <iostream>
#include <string.h>
#include "sha1.h"

bool isEmpty(Uint8* buf, size_t size)
{
    return buf[0] == 0 && !memcmp(buf, buf + 1, size - 1);
}

Uint16 swapU16(Uint16 val)
{
    return (val << 8) | (val >> 8 );
}

Int16 swap16(Int16 val)
{
    return (val << 8) | ((val >> 8) & 0xFF);
}

Uint32 swapU32(Uint32 val)
{
    val = ((val << 8) & 0xFF00FF00 ) | ((val >> 8) & 0xFF00FF );
    return (val << 16) | (val >> 16);
}

Int32 swap32(Int32 val)
{
    val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF );
    return (Uint32)(val << 16) | ((val >> 16) & 0xFFFF);
}

Int64 swap64(Int64 val)
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

Uint64 swapU64(Uint64 val)
{
    return (Uint64)swap64(val);
}

Uint8* getSha1( Uint8 * stuff, Uint32 stuff_size )
{
	SHA1Context sha;
	SHA1Reset( &sha );
    SHA1Input( &sha, (const Uint8*)stuff, stuff_size );
	if( !SHA1Result( &sha ) )
	{
	    std::cout << "getSha1 -> error" << std::endl;
		return 0;
	}

    Uint8* ret = new Uint8[20];
	memset(ret, 0, 20);

	for( int i = 0; i < 5 ; i++ )
	{
	    int val = sha.Message_Digest[ i ];
        if (!isSystemBigEndian())
            val = swap32(val);

		memcpy( (char*)ret + ( i * 4 ), &val, 4 );
	}

	return ret;
}

bool isSystemBigEndian()
{
    char* test = (char*)"\xFE\xFF";
    return (*(unsigned short*)test == 0xFEFF);
}
