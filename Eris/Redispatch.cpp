#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <Eris/redispatch.h>
#include <Eris/Connection.h>

namespace Eris
{

void Redispatch::post()
{
    m_con->postForDispatch(m_obj);
    delete this;
}

}
