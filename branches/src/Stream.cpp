#include "Stream.hpp"
#include "IOException.hpp"
#include <string.h>

Stream::Stream() :
    m_position(0),
    m_length(0),
    m_data(NULL)
{}

Stream::Stream(Int8* data, Int64 length) :
    m_position(0)
{
    if (length <= 0)
        throw "Length cannot be <= to 0";
    m_length = length;
    if (data)
        m_data = data;
    else
    {
        m_data = new Int8[m_length];
        memset(m_data, 0, m_length);
    }
}

Stream::Stream(Int64 length) :
    m_position(0),
    m_length(length)
{
    m_data = new Int8[m_length];
    memset(m_data, 0, m_length);
}

Stream::Stream(const Stream& stream)
{
    if (m_data)
        delete[] m_data;

    m_data = stream.m_data;
    m_position = stream.m_position;
    m_length = stream.m_length;
}

Stream::~Stream()
{
    if (m_data)
        delete[] m_data;
}

void Stream::WriteByte(Int8 byte)
{
    if (m_position > m_length)
        throw IOException("Position passed stream bounds: " + m_position);
    *(Int8*)(m_data + m_position) = byte;
    m_position++;
}

void Stream::WriteBytes(Int8* data, Int64 length)
{
    if (m_position > m_length)
        throw IOException("Position passed stream bounds: " + m_position);

    memcpy(m_data, (const Int8*)(data + m_position), length);
    m_position+= length;
}


Int8 Stream::ReadByte()
{
    if (m_position > m_length)
        throw IOException("Position passed stream bounds: " + m_position);

    return *(Int8*)(m_data + m_position);
}

Int8* Stream::ReadBytes(Int64 length)
{
    if (m_position > m_length)
        throw IOException("Position passed stream bounds: " + m_position);

    Int8* ret = new Int8[length];
    memcpy(ret, (const Int8*)(m_data + m_position), length);
    return ret;
}

void Stream::Seek(Int64 position, SeekOrigin origin)
{
    if (m_position > m_length)
        throw "Position passed stream bounds: " + position;
    switch (origin)
    {
        case Beginning:
            m_position = position;
            break;
        case Current:
            m_position += position;
            break;
        case End:
            m_position += m_length + position;
            break;
    }
}

Int64 Stream::Length()
{
    return m_length;
}

Int64 Stream::Position()
{
    return m_position;
}

bool Stream::AtEnd()
{
    return m_position == m_length;
}
