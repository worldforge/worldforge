#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include <Eris/TypeBoundRedispatch.h>
#include <Eris/Connection.h>
#include <Eris/TypeService.h>
#include <Eris/TypeInfo.h>

#include <Atlas/Objects/Decoder.h>
#include <Atlas/Objects/Operation.h>
#include <sigc++/object_slot.h>

using namespace Atlas::Objects::Operation;
using Atlas::Objects::Root;
using Atlas::Objects::Entity::RootEntity;
using Atlas::Objects::smart_dynamic_cast;

namespace Eris
{

/** recoder is a tempory Atlas bridge / decoder / factory stack we 
 create when the redispatch fires. By sending our original decoded data
 back over the bridge, we run the objects factory on the 'original' data
 sent over the wire, and hopefully new and better objects. */
class Recoder : public Atlas::Objects::ObjectsDecoder
{
public:
    Root m_recoded;
protected:
    void objectArrived(const Root& obj)
    {
        m_recoded = obj;
    }
};

TypeBoundRedispatch::TypeBoundRedispatch(Connection* con, 
        const Root& obj, 
        const TypeInfoSet& unbound) :
    Redispatch(con, obj),
    m_con(con),
    m_unbound(unbound)
{
    for (TypeInfoSet::const_iterator U=m_unbound.begin(); U != m_unbound.end(); ++U) {
        assert((*U)->isBound() == false);
        (*U)->Bound.connect(SigC::slot(*this, &TypeBoundRedispatch::onBound));
    }
    
    con->getTypeService()->BadType.connect(SigC::slot(*this, &TypeBoundRedispatch::onBadType));
}
    
void TypeBoundRedispatch::onBound(TypeInfo* bound)
{
    assert(m_unbound.count(bound));
    m_unbound.erase(bound);
    
    if (m_unbound.empty()) {
        Recoder r;
        r.streamBegin();
        r.streamMessage();
        m_obj->sendContents(r);
        r.mapEnd();
    
        assert(!r.m_recoded->isDefaultObjtype());
        postModified(r.m_recoded);
    }
}

void TypeBoundRedispatch::onBadType(TypeInfo* bad)
{
    assert(m_unbound.count(bad));
    fail();
}

} // of Eris namespace
