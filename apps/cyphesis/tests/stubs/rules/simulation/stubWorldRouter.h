// AUTOGENERATED file, created by the tool generate_stub.py, don't edit!
// If you want to add your own functionality, instead edit the stubWorldRouter_custom.h file.

#ifndef STUB_RULES_SIMULATION_WORLDROUTER_H
#define STUB_RULES_SIMULATION_WORLDROUTER_H

#include "rules/simulation/WorldRouter.h"
#include "stubWorldRouter_custom.h"

#ifndef STUB_WorldRouter_shouldBroadcastPerception
//#define STUB_WorldRouter_shouldBroadcastPerception
  bool WorldRouter::shouldBroadcastPerception(const Atlas::Objects::Operation::RootOperation&) const
  {
    return false;
  }
#endif //STUB_WorldRouter_shouldBroadcastPerception

#ifndef STUB_WorldRouter_deliverTo
//#define STUB_WorldRouter_deliverTo
  void WorldRouter::deliverTo(const Atlas::Objects::Operation::RootOperation&, Ref<LocatedEntity>)
  {
    
  }
#endif //STUB_WorldRouter_deliverTo

#ifndef STUB_WorldRouter_resumeWorld
//#define STUB_WorldRouter_resumeWorld
  void WorldRouter::resumeWorld()
  {
    
  }
#endif //STUB_WorldRouter_resumeWorld

#ifndef STUB_WorldRouter_resolveDispatchTimeForOp
//#define STUB_WorldRouter_resolveDispatchTimeForOp
  void WorldRouter::resolveDispatchTimeForOp(Atlas::Objects::Operation::RootOperationData& op)
  {
    
  }
#endif //STUB_WorldRouter_resolveDispatchTimeForOp

#ifndef STUB_WorldRouter_WorldRouter
//#define STUB_WorldRouter_WorldRouter
   WorldRouter::WorldRouter(Ref<LocatedEntity> baseEntity, EntityCreator& entityCreator, TimeProviderFnType timeProviderFn)
    : BaseWorld(baseEntity, entityCreator, timeProviderFn)
  {
    
  }
#endif //STUB_WorldRouter_WorldRouter

#ifndef STUB_WorldRouter_WorldRouter_DTOR
//#define STUB_WorldRouter_WorldRouter_DTOR
   WorldRouter::~WorldRouter()
  {
    
  }
#endif //STUB_WorldRouter_WorldRouter_DTOR

#ifndef STUB_WorldRouter_shutdown
//#define STUB_WorldRouter_shutdown
  void WorldRouter::shutdown()
  {
    
  }
#endif //STUB_WorldRouter_shutdown

#ifndef STUB_WorldRouter_addEntity
//#define STUB_WorldRouter_addEntity
  void WorldRouter::addEntity(const Ref<LocatedEntity>& obj, const Ref<LocatedEntity>& parent)
  {
    
  }
#endif //STUB_WorldRouter_addEntity

#ifndef STUB_WorldRouter_addNewEntity
//#define STUB_WorldRouter_addNewEntity
  Ref<LocatedEntity> WorldRouter::addNewEntity(const std::string& type, const Atlas::Objects::Entity::RootEntity&)
  {
    return *static_cast<Ref<LocatedEntity>*>(nullptr);
  }
#endif //STUB_WorldRouter_addNewEntity

#ifndef STUB_WorldRouter_delEntity
//#define STUB_WorldRouter_delEntity
  void WorldRouter::delEntity(LocatedEntity* obj)
  {
    
  }
#endif //STUB_WorldRouter_delEntity

#ifndef STUB_WorldRouter_getSpawnEntities
//#define STUB_WorldRouter_getSpawnEntities
  const std::set<std::string>& WorldRouter::getSpawnEntities() const
  {
    static std::set<std::string> _static_instance; return _static_instance;
  }
#endif //STUB_WorldRouter_getSpawnEntities

#ifndef STUB_WorldRouter_registerSpawner
//#define STUB_WorldRouter_registerSpawner
  void WorldRouter::registerSpawner(const std::string& id)
  {
    
  }
#endif //STUB_WorldRouter_registerSpawner

#ifndef STUB_WorldRouter_unregisterSpawner
//#define STUB_WorldRouter_unregisterSpawner
  void WorldRouter::unregisterSpawner(const std::string& id)
  {
    
  }
#endif //STUB_WorldRouter_unregisterSpawner

#ifndef STUB_WorldRouter_operation
//#define STUB_WorldRouter_operation
  void WorldRouter::operation(const Atlas::Objects::Operation::RootOperation&, Ref<LocatedEntity>)
  {
    
  }
#endif //STUB_WorldRouter_operation

#ifndef STUB_WorldRouter_message
//#define STUB_WorldRouter_message
  void WorldRouter::message(Atlas::Objects::Operation::RootOperation, LocatedEntity&)
  {
    
  }
#endif //STUB_WorldRouter_message

#ifndef STUB_WorldRouter_findByName
//#define STUB_WorldRouter_findByName
  Ref<LocatedEntity> WorldRouter::findByName(const std::string& name)
  {
    return *static_cast<Ref<LocatedEntity>*>(nullptr);
  }
#endif //STUB_WorldRouter_findByName

#ifndef STUB_WorldRouter_findByType
//#define STUB_WorldRouter_findByType
  Ref<LocatedEntity> WorldRouter::findByType(const std::string& type)
  {
    return *static_cast<Ref<LocatedEntity>*>(nullptr);
  }
#endif //STUB_WorldRouter_findByType

#ifndef STUB_WorldRouter_getOperationsHandler
//#define STUB_WorldRouter_getOperationsHandler
  OperationsDispatcher<LocatedEntity>& WorldRouter::getOperationsHandler()
  {
    return *static_cast<OperationsDispatcher<LocatedEntity>*>(nullptr);
  }
#endif //STUB_WorldRouter_getOperationsHandler


#endif