#ifndef ERIS_ROUTER_H
#define ERIS_ROUTER_H

#include <Atlas/Objects/ObjectsFwd.h>

namespace Eris
{

/** abstract interface for objects that can route Atlas data. */
class Router
{
public:
    typedef enum {
        IGNORED = 0,
        HANDLED
    } RouterResult;
    virtual ~Router();

    virtual RouterResult handleObject(const Atlas::Objects::Root& obj);

    virtual RouterResult handleOperation(const Atlas::Objects::Operation::RootOperation& op);
    virtual RouterResult handleEntity(const Atlas::Objects::Entity::RootEntity& ent);
};

} // of namespace Eris

#endif
