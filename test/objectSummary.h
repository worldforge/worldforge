#ifndef ERIS_TEST_OBJECT_SUMMARY_H
#define ERIS_TEST_OBJECT_SUMMARY_H

#include <iostream>
#include <Atlas/Objects/Root.h>

typedef std::list<std::string> StringList;

class objectSummary
{
public:

    objectSummary(const Atlas::Objects::Root& obj) :
        m_obj(obj)
    {
        ;
    }
    
private:
    friend std::ostream& operator<<(std::ostream& io, const objectSummary& summary);
    
    Atlas::Objects::Root m_obj;
};

std::ostream& operator<<(std::ostream& io, const objectSummary& summary);

#endif // ERIS_TEST_OBJECT_SUMMARY_H
