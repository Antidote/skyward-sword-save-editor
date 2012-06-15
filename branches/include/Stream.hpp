#ifndef __STREAM_HPP__
#define __STREAM_HPP__

#include "Types.hpp"

class Stream
{
public:
    enum SeekOrigin{Beginning = 0,Current,End};

    Stream();
    Stream(Int8* bytes, Int64 length);
    Stream(Int64 length);
    Stream(Stream* stream);
    virtual ~Stream();

    virtual void WriteByte(Int8 byte);
    virtual void WriteBytes(Int8* data, Int64 length);
    virtual Int8 ReadByte();
    virtual Int8* ReadBytes(Int64 length);
    void Seek(Int64 position, SeekOrigin origin = Current);
    Int64 Length();
    Int64 Position();
    bool AtEnd();
protected:
    Int64  m_position;
    Int64  m_length;
    Int8* m_data;
};

#endif // __STREAM_HPP__
