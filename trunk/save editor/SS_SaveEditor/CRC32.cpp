#include "CRC32.h"

CRC32::CRC32()
{
    for( int codes = 0; codes <= 0xFF; codes++ )
    {
        m_crcTable[codes] = Reflect( codes, 8 ) << 24;

        for( int iPos = 0; iPos < 8; iPos++ )
            m_crcTable[codes] = ( m_crcTable[codes] << 1 ) ^ ( m_crcTable[codes] & (1 << 31) ? CRC32_POLYNOMIAL : 0 );
            m_crcTable[codes] = Reflect( m_crcTable[codes], 32 );
    }
}

quint32 CRC32::Reflect( quint32 reflect, char c )
{
    quint32 value = 0;

    for( int pos = 1; pos < ( c + 1 ); pos++ )
    {
        if( reflect & 1 )
            value |= 1 << ( c - pos );

            reflect >>= 1;
    }

    return value;
}

quint32 CRC32::GetCRC32(const unsigned char *data, quint32 pos, quint32 length )
{
    quint32 CRC = 0xFFFFFFFF;
    while( length-- )
        CRC = ( CRC >> 8 ) ^ m_crcTable[( CRC & 0xFF ) ^ data[pos++]];

   return CRC ^ 0xFFFFFFFF;
}

