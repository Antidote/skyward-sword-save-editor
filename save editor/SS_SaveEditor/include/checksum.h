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

#ifndef CHECKSUM_H
#define CHECKSUM_H

#include <QByteArray>

#define CRC32_POLYNOMIAL 0x04C11DB7

class Checksum
{
public:
    Checksum();
    quint32 CRC32(const quint8* data, quint64 pos, quint64 length); //!< Used by Skyward Sword just a basic CRC32 with default Polynomial.
    quint16 checksum16(const quint8* data, quint64 pos, quint64 length); //!< Used by the oracle games.
private:
    quint32 reflect(quint32 reflect, char c);
    quint32 m_crcTable[256];
};

#endif
