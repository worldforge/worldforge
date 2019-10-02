#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include "Router.h"
#include "Exceptions.h"
#include "LogStream.h"

#include <Atlas/Objects/RootEntity.h>
#include <Atlas/Objects/RootOperation.h>

using Atlas::Objects::Root;
using Atlas::Objects::smart_dynamic_cast;
using Atlas::Objects::Operation::RootOperation;
using Atlas::Objects::Entity::RootEntity;

namespace Eris
{

Router::~Router() = default;

Router::RouterResult Router::handleObject(const Root& obj)
{
    if (obj->instanceOf(Atlas::Objects::Operation::ROOT_OPERATION_NO))
        return handleOperation(smart_dynamic_cast<RootOperation>(obj));

    if (obj->instanceOf(Atlas::Objects::Entity::ROOT_ENTITY_NO))
        return handleEntity(smart_dynamic_cast<RootEntity>(obj));

    throw InvalidOperation("router got an object that is not an op or entity");
}

Router::RouterResult Router::handleOperation(const RootOperation& )
{
    warning() << "doing default routing of operation";
    return IGNORED;
}

Router::RouterResult Router::handleEntity(const RootEntity& )
{
    warning() << "doing default routing of entity";
    return IGNORED;
}

} // of namespace Eris
