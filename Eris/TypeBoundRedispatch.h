#ifndef ERIS_TYPE_BOUND_REDISPATCH_H
#define ERIS_TYPE_BOUND_REDISPATCH_H

#include "Redispatch.h"
#include <set>

namespace Eris
{

class TypeInfo;
typedef std::set<TypeInfo*> TypeInfoSet;

class TypeBoundRedispatch : public Redispatch
{
public:
    TypeBoundRedispatch(Connection* con, const Atlas::Objects::Root& obj, TypeInfo* unbound);    
    TypeBoundRedispatch(Connection* con, const Atlas::Objects::Root& obj, TypeInfoSet unbound);
    void onBound(TypeInfo* bound);    
    void onBadType(TypeInfo* bad);    
private:
    Connection* m_con;
    TypeInfoSet m_unbound;
};

}

#endif
