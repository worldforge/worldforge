// AUTOGENERATED file, created by the tool generate_stub.py, don't edit!
// If you want to add your own functionality, instead edit the stubAwareMind_custom.h file.

#ifndef STUB_RULES_AI_AWAREMIND_H
#define STUB_RULES_AI_AWAREMIND_H

#include "rules/ai/AwareMind.h"
#include "stubAwareMind_custom.h"

#ifndef STUB_AwareMind_AwareMind
//#define STUB_AwareMind_AwareMind
   AwareMind::AwareMind(RouterId mind_id, std::string entity_id, TypeStore& typeStore, SharedTerrain& sharedTerrain, AwarenessStoreProvider& awarenessStoreProvider)
    : BaseMind(mind_id, entity_id, typeStore, sharedTerrain, awarenessStoreProvider)
    , mAwarenessStore(nullptr)
  {
    
  }
#endif //STUB_AwareMind_AwareMind

#ifndef STUB_AwareMind_AwareMind_DTOR
//#define STUB_AwareMind_AwareMind_DTOR
   AwareMind::~AwareMind()
  {
    
  }
#endif //STUB_AwareMind_AwareMind_DTOR

#ifndef STUB_AwareMind_entityAdded
//#define STUB_AwareMind_entityAdded
  void AwareMind::entityAdded(MemEntity& entity)
  {
    
  }
#endif //STUB_AwareMind_entityAdded

#ifndef STUB_AwareMind_entityUpdated
//#define STUB_AwareMind_entityUpdated
  void AwareMind::entityUpdated(MemEntity& entity, const Atlas::Objects::Entity::RootEntity& ent, LocatedEntity* oldLocation)
  {
    
  }
#endif //STUB_AwareMind_entityUpdated

#ifndef STUB_AwareMind_entityDeleted
//#define STUB_AwareMind_entityDeleted
  void AwareMind::entityDeleted(MemEntity& entity)
  {
    
  }
#endif //STUB_AwareMind_entityDeleted

#ifndef STUB_AwareMind_updatePath
//#define STUB_AwareMind_updatePath
  int AwareMind::updatePath()
  {
    return 0;
  }
#endif //STUB_AwareMind_updatePath

#ifndef STUB_AwareMind_getSteering
//#define STUB_AwareMind_getSteering
  Steering* AwareMind::getSteering()
  {
    return nullptr;
  }
#endif //STUB_AwareMind_getSteering

#ifndef STUB_AwareMind_getAwareness
//#define STUB_AwareMind_getAwareness
  const std::shared_ptr<Awareness>& AwareMind::getAwareness() const
  {
    return *static_cast<const std::shared_ptr<Awareness>*>(nullptr);
  }
#endif //STUB_AwareMind_getAwareness

#ifndef STUB_AwareMind_getCurrentServerTime
//#define STUB_AwareMind_getCurrentServerTime
  double AwareMind::getCurrentServerTime() const
  {
    return 0;
  }
#endif //STUB_AwareMind_getCurrentServerTime

#ifndef STUB_AwareMind_setOwnEntity
//#define STUB_AwareMind_setOwnEntity
  void AwareMind::setOwnEntity(OpVector& res, Ref<MemEntity> ownEntity)
  {
    
  }
#endif //STUB_AwareMind_setOwnEntity

#ifndef STUB_AwareMind_processMove
//#define STUB_AwareMind_processMove
  void AwareMind::processMove(OpVector& res)
  {
    
  }
#endif //STUB_AwareMind_processMove

#ifndef STUB_AwareMind_processNavmesh
//#define STUB_AwareMind_processNavmesh
  void AwareMind::processNavmesh()
  {
    
  }
#endif //STUB_AwareMind_processNavmesh

#ifndef STUB_AwareMind_requestAwareness
//#define STUB_AwareMind_requestAwareness
  void AwareMind::requestAwareness(const MemEntity& entity)
  {
    
  }
#endif //STUB_AwareMind_requestAwareness

#ifndef STUB_AwareMind_parseTerrain
//#define STUB_AwareMind_parseTerrain
  void AwareMind::parseTerrain(const Atlas::Message::Element& terrainElement)
  {
    
  }
#endif //STUB_AwareMind_parseTerrain


#endif