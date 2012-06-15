#include "BinaryWriter.hpp"
#include "IOException.hpp"
#include "FileNotFoundException.hpp"

#include <stdio.h>

BinaryWriter::BinaryWriter(Stream* stream) :
    Stream(*stream)
{}

Int8 BinaryWriter::ReadByte()
{
    throw IOException("Stream not open for reading");
}

Int8* BinaryWriter::ReadBytes(Int64)
{
    throw IOException("Stream not open for reading");
}

void BinaryWriter::WriteInt16(Int16 val)
{
    if (m_position > m_length || m_position <= 0)
        throw IOException("Position outside stream bounds");

    *(Int16*)(m_data + m_position) = val;
    m_position += sizeof(Int16);
}

void BinaryWriter::WriteUInt16(Uint16 val)
{
    if (m_position > m_length || m_position <= 0)
        throw IOException("Position outside stream bounds");

    *(Uint16*)(m_data + m_position) = val;
    m_position += sizeof(Uint16);
}

void BinaryWriter::WriteInt32(Int32 val)
{
    if (m_position > m_length || m_position <= 0)
        throw IOException("Position outside stream bounds");

    *(Int32*)(m_data + m_position) = val;
    m_position += sizeof(Int32);
}

void BinaryWriter::WriteUInt32(Uint32 val)
{
    if (m_position > m_length || m_position <= 0)
        throw IOException("Position outside stream bounds");

    *(Uint32*)(m_data + m_position) = val;
    m_position += sizeof(Uint32);
}

void BinaryWriter::WriteInt64(Int64 val)
{
    if (m_position > m_length || m_position <= 0)
        throw IOException("Position outside stream bounds");

    *(Int64*)(m_data + m_position) = val;
    m_position += sizeof(Int64);
}

void BinaryWriter::WriteUInt64(Uint64 val)
{
    if (m_position > m_length || m_position <= 0)
        throw IOException("Position outside stream bounds");
    *(Uint64*)(m_data + m_position) = val;
    m_position += sizeof(Uint64);
}

void BinaryWriter::WriteFloat(float val)
{
    if (m_position > m_length || m_position <= 0)
        throw IOException("Position outside stream bounds");

    *(float*)(m_data + m_position) = val;
    m_position += sizeof(float);
}

void BinaryWriter::WriteDouble(double val)
{
    if (m_position > m_length || m_position <= 0)
        throw IOException("Position outside stream bounds");

    *(double*)(m_data + m_position)= val;
    m_position += sizeof(double);
}

void BinaryWriter::WriteBool(bool val)
{
    if (m_position > m_length || m_position <= 0)
        throw IOException("Position outside stream bounds");

    *(bool*)(m_data + m_position) = val;
    m_position += sizeof(bool);
}

