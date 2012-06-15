#ifndef __BINARYREADER_HPP__
#define __BINARYREADER_HPP__

#include "Stream.hpp"
#include <string>

class BinaryReader : public Stream
{
public:
    BinaryReader(const Stream& stream);

    void WriteByte(Int8 byte);
    void WriteBytes(Int8*, Int64);

    Int16 ReadInt16();
    Uint16 ReadUInt16();
    Int32 ReadInt32();
    Uint32 ReadUInt32();
    Int64 ReadInt64();
    Uint64 ReadUInt64();
    float ReadFloat();
    double ReadDouble();
    bool ReadBool();
};

#endif
