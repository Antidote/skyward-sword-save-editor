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

#ifndef CRC32_H
#define CRC32_H

#include <QByteArray>

#define CRC32_POLYNOMIAL 0x04C11DB7

class CRC32
{
public:
                 CRC32();
                 quint32 GetCRC32(const unsigned char *sData, quint32 pos, quint32 ulLength );

private:
                 quint32 Reflect( quint32 ulReflect, char cChar );
                 quint32 m_crcTable[256];
};

#endif
