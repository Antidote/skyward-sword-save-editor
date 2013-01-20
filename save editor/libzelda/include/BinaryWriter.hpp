#ifndef __BINARYWRITER_HPP__
#define __BINARYWRITER_HPP__

#include "Stream.hpp"
#include <string>

class BinaryWriter : public Stream
{
public:
    BinaryWriter(const Uint8* data, Uint64 length);
    BinaryWriter(const Stream& stream);
    BinaryWriter(const std::string& filename);
    void save(const std::string& filename="");

    Int8 readByte();
    Int8* readBytes(Int64);

    void writeInt16(Int16);
    void writeUInt16(Uint16);
    void writeInt32(Int32);
    void writeUInt32(Uint32);
    void writeInt64(Int64);
    void writeUInt64(Uint64);
    void writeFloat(float);
    void writeDouble(double);
    void writeBool(bool);
    void writeUnicode(const std::string& str);
    bool isOpenForReading();
private:
    std::string m_filename;
};

#endif
