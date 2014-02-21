#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include "Factory.h"

namespace Eris {

Factory::~Factory()
{
}

int Factory::priority()
{
    return 0;
}

} // of namespace Eris
