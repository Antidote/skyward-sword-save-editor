#include "FileStream.hpp"
#include <stdio.h>
#include "FileNotFoundException.hpp"
#include "IOException.hpp"

FileStream::FileStream(const std::string& filename, FileMode fileMode, AccessMode accessMode) :
    m_filename(filename),
    m_filemode(fileMode),
    m_accessmode(accessMode)
{
    FILE* in;
    int length;
    in = fopen(filename.c_str(), "rb");

    if (!in)
    {
        if((fileMode & Create) != Create)
            throw FileNotFoundException(filename.c_str());

        in = fopen(filename.c_str(), "w+b");
    }

    fseek(in, 0, SEEK_END);
    length = ftell(in);
    fseek(in, 0, SEEK_SET);
    m_data = new Uint8[length];

    fread(m_data, 1, length, in);
    fclose(in);
    printf("%i\n", length);
    m_length = length;
    m_position = 0;
}
