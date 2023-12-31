// AUTOGENERATED file, created by the tool generate_stub.py, don't edit!
// If you want to add your own functionality, instead edit the stubArchetypeFactory_custom.h file.

#ifndef STUB_SERVER_ARCHETYPEFACTORY_H
#define STUB_SERVER_ARCHETYPEFACTORY_H

#include "server/ArchetypeFactory.h"
#include "stubArchetypeFactory_custom.h"

#ifndef STUB_ArchetypeFactory_ArchetypeFactory
//#define STUB_ArchetypeFactory_ArchetypeFactory
   ArchetypeFactory::ArchetypeFactory(ArchetypeFactory& o)
    : EntityKit(o)
    , m_parent(nullptr)
  {
    
  }
#endif //STUB_ArchetypeFactory_ArchetypeFactory

#ifndef STUB_ArchetypeFactory_createEntity
//#define STUB_ArchetypeFactory_createEntity
  Ref<Entity> ArchetypeFactory::createEntity(RouterId id, EntityCreation& entityCreation, std::map<std::string, EntityCreation>& entities)
  {
    return *static_cast<Ref<Entity>*>(nullptr);
  }
#endif //STUB_ArchetypeFactory_createEntity

#ifndef STUB_ArchetypeFactory_sendThoughts
//#define STUB_ArchetypeFactory_sendThoughts
  void ArchetypeFactory::sendThoughts(LocatedEntity& entity, std::vector<Atlas::Message::Element>& thoughts)
  {
    
  }
#endif //STUB_ArchetypeFactory_sendThoughts

#ifndef STUB_ArchetypeFactory_sendInitialSight
//#define STUB_ArchetypeFactory_sendInitialSight
  void ArchetypeFactory::sendInitialSight(LocatedEntity& entity)
  {
    
  }
#endif //STUB_ArchetypeFactory_sendInitialSight

#ifndef STUB_ArchetypeFactory_isEntityRefAttribute
//#define STUB_ArchetypeFactory_isEntityRefAttribute
  bool ArchetypeFactory::isEntityRefAttribute(const Atlas::Message::Element& attr) const
  {
    return false;
  }
#endif //STUB_ArchetypeFactory_isEntityRefAttribute

#ifndef STUB_ArchetypeFactory_resolveEntityReference
//#define STUB_ArchetypeFactory_resolveEntityReference
  void ArchetypeFactory::resolveEntityReference(std::map<std::string, EntityCreation>& entities, Atlas::Message::Element& attr)
  {
    
  }
#endif //STUB_ArchetypeFactory_resolveEntityReference

#ifndef STUB_ArchetypeFactory_processResolvedAttributes
//#define STUB_ArchetypeFactory_processResolvedAttributes
  void ArchetypeFactory::processResolvedAttributes(std::map<std::string, EntityCreation>& entities)
  {
    
  }
#endif //STUB_ArchetypeFactory_processResolvedAttributes

#ifndef STUB_ArchetypeFactory_parseEntities
//#define STUB_ArchetypeFactory_parseEntities
  bool ArchetypeFactory::parseEntities(const std::map<std::string, Atlas::Message::MapType>& entitiesElement, std::map<std::string, EntityCreation>& entities)
  {
    return false;
  }
#endif //STUB_ArchetypeFactory_parseEntities

#ifndef STUB_ArchetypeFactory_parseEntities
//#define STUB_ArchetypeFactory_parseEntities
  bool ArchetypeFactory::parseEntities(const Atlas::Message::ListType& entitiesElement, std::map<std::string, EntityCreation>& entities)
  {
    return false;
  }
#endif //STUB_ArchetypeFactory_parseEntities

#ifndef STUB_ArchetypeFactory_createOriginLocationThought
//#define STUB_ArchetypeFactory_createOriginLocationThought
  std::vector<Atlas::Message::Element> ArchetypeFactory::createOriginLocationThought(const LocatedEntity& entity)
  {
    return std::vector<Atlas::Message::Element>();
  }
#endif //STUB_ArchetypeFactory_createOriginLocationThought

#ifndef STUB_ArchetypeFactory_ArchetypeFactory
//#define STUB_ArchetypeFactory_ArchetypeFactory
   ArchetypeFactory::ArchetypeFactory(EntityBuilder& entityBuilder)
    : EntityKit(entityBuilder)
    , m_parent(nullptr)
  {
    
  }
#endif //STUB_ArchetypeFactory_ArchetypeFactory

#ifndef STUB_ArchetypeFactory_ArchetypeFactory_DTOR
//#define STUB_ArchetypeFactory_ArchetypeFactory_DTOR
   ArchetypeFactory::~ArchetypeFactory()
  {
    
  }
#endif //STUB_ArchetypeFactory_ArchetypeFactory_DTOR

#ifndef STUB_ArchetypeFactory_newEntity
//#define STUB_ArchetypeFactory_newEntity
  Ref<Entity> ArchetypeFactory::newEntity(RouterId id, const Atlas::Objects::Entity::RootEntity& attributes)
  {
    return *static_cast<Ref<Entity>*>(nullptr);
  }
#endif //STUB_ArchetypeFactory_newEntity

#ifndef STUB_ArchetypeFactory_duplicateFactory
//#define STUB_ArchetypeFactory_duplicateFactory
  std::unique_ptr<ArchetypeFactory> ArchetypeFactory::duplicateFactory()
  {
    return *static_cast<std::unique_ptr<ArchetypeFactory>*>(nullptr);
  }
#endif //STUB_ArchetypeFactory_duplicateFactory

#ifndef STUB_ArchetypeFactory_addProperties
//#define STUB_ArchetypeFactory_addProperties
  void ArchetypeFactory::addProperties(const PropertyManager& propertyManager)
  {
    
  }
#endif //STUB_ArchetypeFactory_addProperties

#ifndef STUB_ArchetypeFactory_updateProperties
//#define STUB_ArchetypeFactory_updateProperties
  void ArchetypeFactory::updateProperties(std::map<const TypeNode*, TypeNode::PropertiesUpdate>& changes, const PropertyManager& propertyManager)
  {
    
  }
#endif //STUB_ArchetypeFactory_updateProperties


#endif