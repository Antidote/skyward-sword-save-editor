#ifndef __BINARYWRITER_HPP__
#define __BINARYWRITER_HPP__

#include "Stream.hpp"
#include <string>

class BinaryWriter : public Stream
{
public:
    BinaryWriter(const Stream& stream);
    BinaryWriter(const std::string& filename);

    Int8 ReadByte();
    Int8* ReadBytes(Int64);

    void WriteInt16(Int16);
    void WriteUInt16(Uint16);
    void WriteInt32(Int32);
    void WriteUInt32(Uint32);
    void WriteInt64(Int64);
    void WriteUInt64(Uint64);
    void WriteFloat(float);
    void WriteDouble(double);
    void WriteBool(bool);
};

#endif
