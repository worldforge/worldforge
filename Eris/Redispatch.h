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
    Redispatch(Connection* con, Atlas::Objects::Root obj) :
        m_con(con),
        m_obj(obj)
    {

    }

    void post();

private:
    Connection* m_con;
    Atlas::Objects::Root m_obj;
};

} // of namespace Eris

#endif
