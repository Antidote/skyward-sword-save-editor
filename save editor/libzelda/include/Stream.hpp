#ifndef __STREAM_HPP__
#define __STREAM_HPP__

#include "Types.hpp"

class Stream
{
public:
    enum Endian{ LittleEndian, BigEndian};
    enum SeekOrigin{Beginning = 0,Current,End};

    Stream();
    Stream(const Uint8* bytes, Uint64 length);
    Stream(Int64 length);
    Stream(Stream* stream);
    virtual ~Stream();

    virtual void writeBit(bool val);
    virtual void writeByte(Int8 byte);
    virtual void writeBytes(Int8* data, Int64 length);
    virtual bool readBit();
    virtual Int8 readByte();
    virtual Int8* readBytes(Int64 length);
    void seek(Int64 position, SeekOrigin origin = Current);
    void resize(Uint64 newSize);

    void  setData(const Uint8*, Uint64 length);
    Uint8* data() const;

    Int64 length();
    Int64 position();
    bool atEnd();
    void setAutoResizing(bool val);
    bool autoResizing() const;

    virtual bool isOpenForReading();
    virtual bool isOpenForWriting();

    void setEndianess(Endian endian);
    Endian endianness() const;
    bool isBigEndian() const;
    bool isLittleEndian() const;
protected:
    Uint32  m_bitPosition;
    Uint64  m_position;
    Uint64  m_length;
    Endian  m_endian;
    Uint8*  m_data;
    bool    m_autoResize;
};

#endif // __STREAM_HPP__
