#include <utility>

#ifndef ERIS_REDISPATCH_H
#define ERIS_REDISPATCH_H

#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/SmartPtr.h>

#include <sigc++/trackable.h>

namespace Eris
{

// forward decls
class Connection;

class Redispatch : public sigc::trackable
{
protected:
    Redispatch(Connection* con, Atlas::Objects::Root obj) :
        m_obj(std::move(obj)),
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
