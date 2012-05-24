#include "BinaryReader.hpp"
#include "IOException.hpp"
#include "FileNotFoundException.hpp"

#include <stdio.h>

BinaryReader::BinaryReader(const Stream& stream) :
    Stream(stream)
{}

BinaryReader::BinaryReader(const std::string& filename)
{
    FILE* in;
    int length;

    in = fopen(filename.c_str(), "rb");
    if (!in)
        throw FileNotFoundException(filename.c_str());
    fseek(in, 0, SEEK_END);
    length = ftell(in);
    fseek(in, 0, SEEK_SET);
    m_data = new Int8[length];

    fread(m_data, 1, length, in);
    m_length = length;
    m_position = 0;
}

Int16 BinaryReader::ReadInt16()
{
    Int16 ret = *(Int16*)(m_data + m_position);
    m_position += sizeof(Int16);
    return ret;
}

Uint16 BinaryReader::ReadUInt16()
{
    Uint16 ret = *(Uint16*)(m_data + m_position);
    m_position += sizeof(Uint16);
    return ret;
}

Int32 BinaryReader::ReadInt32()
{
    Int32 ret = *(Int32*)(m_data + m_position);
    m_position += sizeof(Int32);
    return ret;
}

Uint32 BinaryReader::ReadUInt32()
{
    if (m_position > m_length)
        throw IOException("Position passed stream bounds: " + m_position);

    Uint32 ret = *(Uint32*)(m_data + m_position);
    m_position += sizeof(Uint32);
    return ret;
}

Int64 BinaryReader::ReadInt64()
{
    if (m_position > m_length)
        throw IOException("Position passed stream bounds: " + m_position);

    Int64 ret = *(Int64*)(m_data + m_position);
    m_position += sizeof(Int64);
    return ret;
}

Uint64 BinaryReader::ReadUInt64()
{
    if (m_position > m_length)
        throw IOException("Position passed stream bounds: " + m_position);
    Uint64 ret = *(Uint64*)(m_data + m_position);
    m_position += sizeof(Uint64);
    return ret;
}

float BinaryReader::ReadFloat()
{
    if (m_position > m_length)
        throw IOException("Position passed stream bounds: " + m_position);

    float ret = *(float*)(m_data + m_position);
    m_position += sizeof(float);
    return ret;
}

double BinaryReader::ReadDouble()
{
    if (m_position > m_length)
        throw IOException("Position passed stream bounds: " + m_position);

    double ret = *(double*)(m_data + m_position);
    m_position += sizeof(double);

    return ret;
}

bool BinaryReader::ReadBool()
{
    if (m_position > m_length)
        throw IOException("Position passed stream bounds: " + m_position);

    bool ret = *(bool*)(m_data + m_position);
    m_position += sizeof(bool);
    return ret;
}

