#ifndef __FILESTREAM_HPP__
#define __FILESTREAM_HPP__

#include "Stream.hpp"
#include <string>

class FileStream : public Stream
{
public:
    enum FileMode { Open, Create, OpenOrCreate = Open|Create, Truncate, Append };
    enum AccessMode { ReadOnly, WriteOnly, ReadWrite };
    FileStream(const std::string& filename, FileMode fileMode, AccessMode accessMode);


private:
    std::string m_filename;
    FileMode m_filemode;
    AccessMode m_accessmode;
};

#endif
