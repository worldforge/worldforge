#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <Eris/Redispatch.h>
#include <Eris/Connection.h>
#include <Eris/LogStream.h>

namespace Eris
{

void Redispatch::post()
{
    debug() << "posting redispatch";
    m_con->postForDispatch(m_obj);
    delete this;
}

void Redispatch::postModified(const Atlas::Objects::Root& obj)
{
    debug() << "posting mutated redispatch";
    m_con->postForDispatch(obj);
    delete this;
}

void Redispatch::fail()
{
    debug() << "redispatch failed";
}

}
