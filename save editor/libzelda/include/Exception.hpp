#ifndef __EXCEPTION_HPP__
#define __EXCEPTION_HPP__

#include <string>

class Exception
{
public:
    Exception(const std::string& message) :
        m_message(message)
    {};

    std::string message() const
    {
        return m_message;
    };
protected:
    std::string m_message;
};

#endif
