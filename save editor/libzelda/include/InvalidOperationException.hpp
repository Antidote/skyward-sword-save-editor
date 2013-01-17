#ifndef __INVALID_OPERATION_EXCEPTION_HPP__
#define __INVALID_OPERATION_EXCEPTION_HPP__

#include <string>
#include <Exception.hpp>

class InvalidOperationException : public Exception
{
public:
    InvalidOperationException(const std::string& error)
        : Exception("InvalidOperationException:\n" + error)
    {
    }
};
#endif // __INVALID_OPERATION_EXCEPTION_HPP__
