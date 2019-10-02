#ifndef ERIS_TEST_UTILS_H
#define ERIS_TEST_UTILS_H

#include <exception>
#include <string>
#include <Atlas/Objects/ObjectsFwd.h>

namespace Eris
{
    class Connection;
}

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

namespace Eris
{

class TestInjector
{
public:
    TestInjector(Connection* con) :
        m_con(con)
    {
    }

    void inject(const Atlas::Objects::Operation::RootOperation& op);
private:
    Connection* m_con;
};

}

#endif
