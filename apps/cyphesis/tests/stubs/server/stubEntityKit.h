// AUTOGENERATED file, created by the tool generate_stub.py, don't edit!
// If you want to add your own functionality, instead edit the stubEntityKit_custom.h file.

#ifndef STUB_SERVER_ENTITYKIT_H
#define STUB_SERVER_ENTITYKIT_H

#include "server/EntityKit.h"
#include "stubEntityKit_custom.h"

#ifndef STUB_EntityKit_newEntity
//#define STUB_EntityKit_newEntity
  Ref<Entity> EntityKit::newEntity(RouterId id, const Atlas::Objects::Entity::RootEntity& attributes)
  {
    return *static_cast<Ref<Entity>*>(nullptr);
  }
#endif //STUB_EntityKit_newEntity

#ifndef STUB_EntityKit_addProperties
//#define STUB_EntityKit_addProperties
  void EntityKit::addProperties(const PropertyManager& propertyManager)
  {
    
  }
#endif //STUB_EntityKit_addProperties

#ifndef STUB_EntityKit_updateProperties
//#define STUB_EntityKit_updateProperties
  void EntityKit::updateProperties(std::map<const TypeNode*, TypeNode::PropertiesUpdate>& changes, const PropertyManager& propertyManager)
  {
    
  }
#endif //STUB_EntityKit_updateProperties


#endif