#ifndef ERIS_REDISPATCH_H
#define ERIS_REDISPATCH_H

#include <sigc++/object.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Operation.h>

namespace Eris
{

// forward decls
class Connection;

class Redispatch : public SigC::Object
{
protected:
    Redispatch(Connection* con, const Atlas::Objects::Root& obj) :
        m_obj(obj),
        m_con(con)
    {

    }

    void post();

    void postModified(const Atlas::Objects::Root& obj);

    void fail();

    const Atlas::Objects::Root m_obj;
    
private:
    Connection* m_con;
};

} // of namespace Eris

#endif
