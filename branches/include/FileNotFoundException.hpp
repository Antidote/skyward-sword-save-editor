#ifndef __FILENOTFOUNDEXCEPTION_HPP__
#define __FILENOTFOUNDEXCEPTION_HPP__

#include "Exception.hpp"

class FileNotFoundException : public Exception
{
public:
    FileNotFoundException(const std::string& filename) :
        Exception("Couldn't not find \"" + filename + "\", please check that it exists."),
        m_filename(filename)
    {}
private:
    std::string m_filename;
};

#endif
