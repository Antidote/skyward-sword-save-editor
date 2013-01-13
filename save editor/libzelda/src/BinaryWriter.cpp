#include "BinaryWriter.hpp"
#include "IOException.hpp"
#include "FileNotFoundException.hpp"
#include "utility.h"

#include <stdio.h>
#include <string.h>

BinaryWriter::BinaryWriter(const Stream& stream) :
    Stream(stream)
{}

BinaryWriter::BinaryWriter(const std::string& filename)
    : m_filename(filename)
{
    m_length = 0x10;
    m_position = 0;
    m_data = new Uint8[m_length];
    memset(m_data, 0, m_length);
}

void BinaryWriter::Save(const std::string& filename)
{
    if (filename.empty() && m_filename.empty())
        throw Exception("InvalidOperationException: BinaryWriter::Save() -> No file specified, cannot save.");

    if (!filename.empty())
        m_filename = filename;

    FILE* out = fopen(m_filename.c_str(), "wb");
    if (!out)
        throw FileNotFoundException(m_filename);

    fwrite(m_data, 1, m_length, out);
    fclose(out);
}

Int8 BinaryWriter::readByte()
{
    throw IOException("Stream not open for reading");
}

Int8* BinaryWriter::readBytes(Int64)
{
    throw IOException("Stream not open for reading");
}

void BinaryWriter::writeInt16(Int16 val)
{
    if (m_position + sizeof(Int16) > m_length && m_autoResize)
        resize(m_position + 2);
    else if (m_position > m_length)
        throw IOException("BinaryWriter::WriteInt16() -> Position outside stream bounds");

    if ((!isSystemBigEndian() && m_endian == Stream::BigEndian) || (isSystemBigEndian() && m_endian == Stream::LittleEndian))
        val = swap16(val);

    *(Int16*)(m_data + m_position) = val;
    m_position += sizeof(Int16);
}

void BinaryWriter::writeUInt16(Uint16 val)
{
    if (m_position > m_length && m_autoResize)
        resize(m_position + 2);
    else if (m_position > m_length)
        throw IOException("BinaryWriter::WriteUInt16() -> Position outside stream bounds");

    if ((!isSystemBigEndian() && m_endian == Stream::BigEndian) || (isSystemBigEndian() && m_endian == Stream::LittleEndian))
        val = swapU16(val);

    *(Uint16*)(m_data + m_position) = val;
    m_position += sizeof(Uint16);
}

void BinaryWriter::writeInt32(Int32 val)
{
    if (m_position + 4 > m_length && m_autoResize)
        resize(m_position + 4);
    else if (m_position > m_length)
        throw IOException("BinaryWriter::WriteInt32() -> Position outside stream bounds");

    if ((!isSystemBigEndian() && m_endian == Stream::BigEndian) || (isSystemBigEndian() && m_endian == Stream::LittleEndian))
        val = swap32(val);

    *(Int32*)(m_data + m_position) = val;
    m_position += sizeof(Int32);
}

void BinaryWriter::writeUInt32(Uint32 val)
{
    if (m_position + 4 > m_length && m_autoResize)
        resize(m_position + 4);
    else if (m_position > m_length)
        throw IOException("BinaryWriter::WriteUInt32() -> Position outside stream bounds");

    if ((!isSystemBigEndian() && m_endian == Stream::BigEndian) || (isSystemBigEndian() && m_endian == Stream::LittleEndian))
        val = swap32(val);

    *(Uint32*)(m_data + m_position) = val;
    m_position += sizeof(Uint32);
}

void BinaryWriter::writeInt64(Int64 val)
{
    if (m_position + 8 > m_length && m_autoResize)
        resize(m_position + 8);
    else if (m_position > m_length)
        throw IOException("BinaryWriter::WriteInt64() -> Position outside stream bounds");

    if ((!isSystemBigEndian() && m_endian == Stream::BigEndian) || (isSystemBigEndian() && m_endian == Stream::LittleEndian))
        val = swap64(val);

    *(Int64*)(m_data + m_position) = val;
    m_position += sizeof(Int64);
}

void BinaryWriter::writeUInt64(Uint64 val)
{
    if (m_position + 8 > m_length && m_autoResize)
        resize(m_position + 8);
    else if (m_position > m_length)
        throw IOException("BinaryWriter::WriteUInt64() -> Position outside stream bounds");


    if ((!isSystemBigEndian() && m_endian == Stream::BigEndian) || (isSystemBigEndian() && m_endian == Stream::LittleEndian))
        val = swap64(val);

    *(Uint64*)(m_data + m_position) = val;
    m_position += sizeof(Uint64);
}

void BinaryWriter::writeFloat(float val)
{
    if (m_position + sizeof(float) > m_length && m_autoResize)
        resize(m_position + sizeof(float));
    else if (m_position > m_length)
        throw IOException("BinaryWriter::WriteFloat() -> Position outside stream bounds");

    *(float*)(m_data + m_position) = val;
    m_position += sizeof(float);
}

void BinaryWriter::writeDouble(double val)
{
    if (m_position  > m_length && m_autoResize)
        resize(m_position + 8);
    else if (m_position > m_length)
        throw IOException("BinaryWriter::WriteDouble() -> Position outside stream bounds");

    *(double*)(m_data + m_position)= val;
    m_position += sizeof(double);
}

void BinaryWriter::writeBool(bool val)
{
    if (m_position + sizeof(bool) > m_length && m_autoResize)
        resize(m_position + sizeof(bool));
    else if (m_position > m_length)
        throw IOException("BinaryWriter::WriteBool() -> Position outside stream bounds");

    *(bool*)(m_data + m_position) = val;
    m_position += sizeof(bool);
}

bool BinaryWriter::isOpenForReading()
{
    return false;
}

