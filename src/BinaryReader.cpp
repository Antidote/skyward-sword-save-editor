#include "BinaryReader.hpp"
#include "IOException.hpp"
#include "FileNotFoundException.hpp"
#include "utility.h"

#include <stdio.h>
#include <stdlib.h>

BinaryReader::BinaryReader(const Stream& stream) :
    Stream(stream)
{}

BinaryReader::BinaryReader(const Uint8* data, Uint64 length) :
    Stream(data, length)
{
}

BinaryReader::BinaryReader(const std::string& filename)
    : m_filename(filename)
{
    FILE* in;
    int length;
    in = fopen(filename.c_str(), "rb");

    if (!in)
        throw FileNotFoundException(filename);

    fseek(in, 0, SEEK_END);
    length = ftell(in);
    fseek(in, 0, SEEK_SET);
    m_data = new Uint8[length];

    fread(m_data, 1, length, in);
    fclose(in);
    m_length = length;
    m_position = 0;
}

void BinaryReader::writeByte(Int8)
{
    throw IOException("BinaryReader::WriteByte() -> Stream not open for writing");
}

void BinaryReader::writeBytes(Int8*, Int64)
{
    throw IOException("BinaryReader::WriteBytes() -> Stream not open for writing");
}

Int16 BinaryReader::readInt16()
{
    if (m_position > m_length)
        throw IOException("BinaryReader::ReadInt16() -> Position outside stream bounds");
    Int16 ret = *(Int16*)(m_data + m_position);
    m_position += 2;

    if ((!isSystemBigEndian() && m_endian == Stream::BigEndian) || (isSystemBigEndian() && m_endian == Stream::LittleEndian))
        ret = swap16(ret);
    return ret;
}

Uint16 BinaryReader::readUInt16()
{
    if (m_position > m_length)
        throw IOException("BinaryReader::ReadUint16() -> Position outside stream bounds");
    Uint16 ret = *(Uint16*)(m_data + m_position);
    m_position += 2;

    if ((!isSystemBigEndian() && m_endian == Stream::BigEndian) || (isSystemBigEndian() && m_endian == Stream::LittleEndian))
        ret = swapU16(ret);

    return ret;
}

Int32 BinaryReader::readInt32()
{
    if (m_position > m_length)
        throw IOException("BinaryReader::ReadUint32() -> Position outside stream bounds");
    Int32 ret = *(Int32*)(m_data + m_position);
    m_position += 4;

    if ((!isSystemBigEndian() && m_endian == Stream::BigEndian) || (isSystemBigEndian() && m_endian == Stream::LittleEndian))
        ret = swap32(ret);
    return ret;
}

Uint32 BinaryReader::readUInt32()
{
    if (m_position > m_length)
        throw IOException("BinaryReader::ReadUint32() -> Position outside stream bounds");

    Uint32 ret = *(Uint32*)(m_data + m_position);
    m_position += 4;

    if ((!isSystemBigEndian() && m_endian == Stream::BigEndian) || (isSystemBigEndian() && m_endian == Stream::LittleEndian))
        ret = swapU32(ret);
    return ret;
}

Int64 BinaryReader::readInt64()
{
    if (m_position > m_length)
        throw IOException("BinaryReader::ReadInt64() -> Position outside stream bounds");

    Int64 ret = *(Int64*)(m_data + m_position);
    m_position += 8;

    if ((!isSystemBigEndian() && m_endian == Stream::BigEndian) || (isSystemBigEndian() && m_endian == Stream::LittleEndian))
        ret = swap64(ret);
    return ret;
}

Uint64 BinaryReader::readUInt64()
{
    if (m_position > m_length)
        throw IOException("BinaryReader::ReadUInt64() -> Position outside stream bounds");
    Uint64 ret = *(Uint64*)(m_data + m_position);
    m_position += 8;

    if ((!isSystemBigEndian() && m_endian == Stream::BigEndian) || (isSystemBigEndian() && m_endian == Stream::LittleEndian))
        ret = swap64(ret);
    return ret;
}

float BinaryReader::readFloat()
{
    if (m_position > m_length)
        throw IOException("BinaryReader::ReadFloat() -> Position outside stream bounds");

    float ret = *(float*)(m_data + m_position);
    m_position += 4;
    return ret;
}

double BinaryReader::readDouble()
{
    if (m_position > m_length)
        throw IOException("BinaryReader::ReadDouble() -> Position outside stream bounds");

    double ret = *(double*)(m_data + m_position);
    m_position += 8;

    if ((!isSystemBigEndian() && m_endian == Stream::BigEndian) || (isSystemBigEndian() && m_endian == Stream::LittleEndian))
        ret = swap64(ret);

    return ret;
}

bool BinaryReader::readBool()
{
    if (m_position > m_length)
        throw IOException("BinaryReader::ReadBool() -> Position outside stream bounds");

    bool ret = *(bool*)(m_data + m_position);
    m_position += 1;
    return ret;
}

bool BinaryReader::isOpenForWriting()
{
    return false;
}

