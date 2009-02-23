#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <Eris/TypeBoundRedispatch.h>
#include <Eris/Connection.h>
#include <Eris/TypeService.h>
#include <Eris/TypeInfo.h>
#include <Eris/LogStream.h>

#include <Atlas/Objects/Operation.h>
#include <sigc++/slot.h>
#include <sigc++/bind.h>

using namespace Atlas::Objects::Operation;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::smart_dynamic_cast;

namespace Eris
{


TypeBoundRedispatch::TypeBoundRedispatch(Connection* con, 
        const Root& obj, 
        TypeInfo* unbound) :
    Redispatch(con, obj),
    m_con(con)
{
    m_unbound.insert(unbound);
    
    assert(unbound->isBound() == false);
    unbound->Bound.connect(sigc::bind(sigc::mem_fun(this, &TypeBoundRedispatch::onBound), unbound));
    
    con->getTypeService()->BadType.connect(sigc::mem_fun(this, &TypeBoundRedispatch::onBadType));
}

TypeBoundRedispatch::TypeBoundRedispatch(Connection* con, 
        const Root& obj, 
        const TypeInfoSet& unbound) :
    Redispatch(con, obj),
    m_con(con),
    m_unbound(unbound)
{
    for (TypeInfoSet::const_iterator U=m_unbound.begin(); U != m_unbound.end(); ++U) {
        assert((*U)->isBound() == false);
        (*U)->Bound.connect(sigc::bind(sigc::mem_fun(this, &TypeBoundRedispatch::onBound), *U));
    }
    
    con->getTypeService()->BadType.connect(sigc::mem_fun(this, &TypeBoundRedispatch::onBadType));
}
    
void TypeBoundRedispatch::onBound(TypeInfo* bound)
{
    assert(m_unbound.count(bound));
    m_unbound.erase(bound);
    
    if (m_unbound.empty()) post();
}

void TypeBoundRedispatch::onBadType(TypeInfo* bad)
{
    if (m_unbound.count(bad)) {
        warning() << "TypeBoundRedispatch was waiting on bad type " << bad->getName(); 
        fail();
    }
}

} // of Eris namespace
