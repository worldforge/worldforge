#ifndef ERIS_TEST_UTILS_H
#define ERIS_TEST_UTILS_H

#include <exception>
#include <string>

class TestFailure : public std::exception
{
public:
    TestFailure(const std::string& what) : 
        m_what(what)
    {
        ;
    }
    
    ~TestFailure() throw()
    {
        ;
    }
    
    virtual const char* what() const throw()
    {
        return m_what.c_str();
    }
private:
    std::string m_what;
};

#endif