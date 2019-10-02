#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <Atlas/Message/Element.h>
#include "testUtils.h"
#include <Eris/Connection.h>

#include <stdexcept>
#include <cassert>

namespace Eris
{

void TestInjector::inject(const Atlas::Objects::Operation::RootOperation& op)
{
    m_con->dispatchOp(op);
}

}
