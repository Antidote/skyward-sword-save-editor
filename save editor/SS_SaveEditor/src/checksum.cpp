// This file is part of WiiKing2 Editor.
//
// WiiKing2 Editor is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Wiiking2 Editor is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with WiiKing2 Editor.  If not, see <http://www.gnu.org/licenses/>

#include "checksum.h"

Checksum::Checksum()
{
    for( int codes = 0; codes <= 0xFF; codes++ )
    {
         m_crcTable[codes] = reflect( codes, 8 ) << 24;

         for( int iPos = 0; iPos < 8; iPos++ )
             m_crcTable[codes] = ( m_crcTable[codes] << 1 ) ^ ( m_crcTable[codes] & (1 << 31) ? CRC32_POLYNOMIAL : 0 );
             m_crcTable[codes] = reflect( m_crcTable[codes], 32 );
    }
}

quint32 Checksum::reflect( quint32 reflect, char c )
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

quint32 Checksum::CRC32(const quint8 *data, quint64 pos, quint64 length )
{
    quint32 CRC = 0xFFFFFFFF;
    while( length-- )
                 CRC = ( CRC >> 8 ) ^ m_crcTable[( CRC & 0xFF ) ^ data[pos++]];

   return CRC ^ 0xFFFFFFFF;
}

quint16 Checksum::checksum16(const quint8 *data, quint64 pos, quint64 length)
{
    quint16 sum = 0;

    for (quint64 i = pos; i < pos + length; i += 2)
        sum += *(quint16*)(data + i);

    return sum;
}

