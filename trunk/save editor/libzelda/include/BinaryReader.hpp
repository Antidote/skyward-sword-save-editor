#ifndef __BINARYREADER_HPP__
#define __BINARYREADER_HPP__

#include "Stream.hpp"
#include <string>

class BinaryReader : public Stream
{
public:
    BinaryReader(const Uint8* data, Uint64 length);
    BinaryReader(const Stream& stream);
    BinaryReader(const std::string& filename);
    void Save(const std::string& filename=NULL);

    void writeByte(Int8 byte);
    void writeBytes(Int8*, Int64);

    Int16       readInt16();
    Uint16      readUInt16();
    Int32       readInt32();
    Uint32      readUInt32();
    Int64       readInt64();
    Uint64      readUInt64();
    float       readFloat();
    double      readDouble();
    bool        readBool();
    std::string readUnicode();

    bool   isOpenForWriting();
private:
    std::string m_filename;
};

#endif
