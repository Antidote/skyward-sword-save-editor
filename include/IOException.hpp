#ifndef __IOEXCEPTION_HPP__
#define __IOEXCEPTION_HPP__

#include "Exception.hpp"

class IOException : public Exception
{
public:
    IOException(const std::string& message) :
    Exception("IOException: " + message)
    {};
};

#endif
