// AUTOGENERATED file, created by the tool generate_stub.py, don't edit!
// If you want to add your own functionality, instead edit the stubThing_custom.h file.

#ifndef STUB_RULES_SIMULATION_THING_H
#define STUB_RULES_SIMULATION_THING_H

#include "rules/simulation/Thing.h"
#include "stubThing_custom.h"

#ifndef STUB_Thing_updateProperties
//#define STUB_Thing_updateProperties
  void Thing::updateProperties(const Operation& op, OpVector& res)
  {
    
  }
#endif //STUB_Thing_updateProperties

#ifndef STUB_Thing_lookAtEntity
//#define STUB_Thing_lookAtEntity
  bool Thing::lookAtEntity(const Operation& op, OpVector& res, const LocatedEntity& watcher) const
  {
    return false;
  }
#endif //STUB_Thing_lookAtEntity

#ifndef STUB_Thing_generateSightOp
//#define STUB_Thing_generateSightOp
  void Thing::generateSightOp(const LocatedEntity& observingEntity, const Operation& originalLookOp, OpVector& res) const
  {
    
  }
#endif //STUB_Thing_generateSightOp

#ifndef STUB_Thing_moveToNewLocation
//#define STUB_Thing_moveToNewLocation
  void Thing::moveToNewLocation(Ref<LocatedEntity>& new_loc, OpVector& res, Domain* existingDomain, const Point3D& newPos, const Quaternion& newOrientation, const Vector3D& newImpulseVelocity)
  {
    
  }
#endif //STUB_Thing_moveToNewLocation

#ifndef STUB_Thing_moveOurselves
//#define STUB_Thing_moveOurselves
  void Thing::moveOurselves(const Operation& op, const Atlas::Objects::Entity::RootEntity& ent, OpVector& res)
  {
    
  }
#endif //STUB_Thing_moveOurselves

#ifndef STUB_Thing_moveOtherEntity
//#define STUB_Thing_moveOtherEntity
  void Thing::moveOtherEntity(const Operation& op, const Atlas::Objects::Entity::RootEntity& ent, OpVector& res)
  {
    
  }
#endif //STUB_Thing_moveOtherEntity

#ifndef STUB_Thing_Thing
//#define STUB_Thing_Thing
   Thing::Thing(RouterId id)
    : Entity(id)
  {
    
  }
#endif //STUB_Thing_Thing

#ifndef STUB_Thing_DeleteOperation
//#define STUB_Thing_DeleteOperation
  void Thing::DeleteOperation(const Operation& op, OpVector&)
  {
    
  }
#endif //STUB_Thing_DeleteOperation

#ifndef STUB_Thing_MoveOperation
//#define STUB_Thing_MoveOperation
  void Thing::MoveOperation(const Operation& op, OpVector&)
  {
    
  }
#endif //STUB_Thing_MoveOperation

#ifndef STUB_Thing_SetOperation
//#define STUB_Thing_SetOperation
  void Thing::SetOperation(const Operation& op, OpVector&)
  {
    
  }
#endif //STUB_Thing_SetOperation

#ifndef STUB_Thing_UpdateOperation
//#define STUB_Thing_UpdateOperation
  void Thing::UpdateOperation(const Operation& op, OpVector&)
  {
    
  }
#endif //STUB_Thing_UpdateOperation

#ifndef STUB_Thing_LookOperation
//#define STUB_Thing_LookOperation
  void Thing::LookOperation(const Operation& op, OpVector&)
  {
    
  }
#endif //STUB_Thing_LookOperation

#ifndef STUB_Thing_ImaginaryOperation
//#define STUB_Thing_ImaginaryOperation
  void Thing::ImaginaryOperation(const Operation& op, OpVector&)
  {
    
  }
#endif //STUB_Thing_ImaginaryOperation

#ifndef STUB_Thing_TalkOperation
//#define STUB_Thing_TalkOperation
  void Thing::TalkOperation(const Operation& op, OpVector&)
  {
    
  }
#endif //STUB_Thing_TalkOperation

#ifndef STUB_Thing_CreateOperation
//#define STUB_Thing_CreateOperation
  void Thing::CreateOperation(const Operation& op, OpVector& res)
  {
    
  }
#endif //STUB_Thing_CreateOperation


#endif