#ifndef __FILENOTFOUNDEXCEPTION_HPP__
#define __FILENOTFOUNDEXCEPTION_HPP__

#include "Exception.hpp"

class FileNotFoundException : public Exception
{
public:
    FileNotFoundException(const std::string& filename) :
        Exception("FileNotFoundException:\nCouldn't not find \"" + filename + "\", please check that it exists."),
        m_filename(filename)
    {}

    std::string filename() const { return m_filename; }
private:
    std::string m_filename;
};

#endif
