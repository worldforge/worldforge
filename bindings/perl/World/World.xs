#include <Eris/World.h>
#include <Eris/Factory.h>
#include <Eris/Entity.h>

#include <sigcperl/signal_wrap.h>

#include "../worldhandle.h"
#include "../perlentity.h"
#include "../conversion.h"
#include "../atlas_convert.h"

extern "C" {
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
}

class PerlFactory : public Eris::Factory
{
 public:
  PerlFactory(SV* factory, Eris::World* world);
  ~PerlFactory();

  virtual bool accept(const Atlas::Objects::Entity::GameEntity &ge, Eris::World *world);
  virtual Eris::EntityPtr instantiate(const Atlas::Objects::Entity::GameEntity &ge, Eris::World *world);

  static PerlFactory* find(SV* factory, Eris::World* world);

 private:

  SV* _factory;
  Eris::World* _world;
  SigCPerl::Slot *_accept_func, *_instantiate_func;

  typedef std::map<std::pair<HV*,Eris::World*>,PerlFactory*> FactoryMap;
  static FactoryMap _factory_map;
};

PerlFactory::FactoryMap PerlFactory::_factory_map;

PerlFactory::PerlFactory(SV* factory, Eris::World* world)
	: _factory(newSVsv(factory)), _world(world)
{
  assert(sv_derived_from(factory, "WorldForge::Eris::Factory"));
  assert(SvTYPE(SvRV(factory)) == SVt_PVHV);

  HV* hash = (HV*) SvRV(factory);

  SV** tmp;
  const char accept_func_name[] = "_Eris_Factory_accept_func";
  const char instantiate_func_name[] = "_Eris_Factory_instantiate_func";

  tmp = hv_fetch(hash, accept_func_name, sizeof(accept_func_name) - 1, 0);
  assert(tmp);
  _accept_func = (SigCPerl::Slot*) SvIV(*tmp);
  assert(_accept_func);

  tmp = hv_fetch(hash, instantiate_func_name, sizeof(instantiate_func_name) - 1, 0);
  assert(tmp);
  _instantiate_func = (SigCPerl::Slot*) SvIV(*tmp);
  assert(_instantiate_func);

  _factory_map[FactoryMap::key_type(hash, world)] = this;
}

PerlFactory::~PerlFactory()
{
  assert(SvTYPE(SvRV(_factory)) == SVt_PVHV);
  HV* hash = (HV*) SvRV(_factory);

  FactoryMap::iterator I = _factory_map.find(FactoryMap::key_type(hash, _world));
  if(I != _factory_map.end())
    _factory_map.erase(I);
}

bool PerlFactory::accept(const Atlas::Objects::Entity::GameEntity &ge,
			 Eris::World *world)
{
  assert(world == _world);

  SigCPerl::Data data;

  data.push_back(ge.AsObject());
  data.push_back(world);

  SigCPerl::Data out_data = _accept_func->call(data, G_SCALAR);
  SigCPerl::Data::Iter I = out_data.begin();
  return (I != out_data.end()) ? SvTRUE(*I) : false;
}

Eris::EntityPtr PerlFactory::instantiate(const Atlas::Objects::Entity::GameEntity &ge,
					 Eris::World *world)
{
  assert(world == _world);

  PerlEntity* entity = new PerlEntity(ge, world);

  SigCPerl::Data data;

  data.push_back(entity->sv());
  data.push_back(ge.AsObject());
  data.push_back(world);

  _instantiate_func->call(data, G_VOID | G_DISCARD);

  return entity;
}

PerlFactory* PerlFactory::find(SV* factory, Eris::World* world)
{
  assert(sv_derived_from(factory, "WorldForge::Eris::Factory"));
  assert(SvTYPE(SvRV(factory)) == SVt_PVHV);

  HV* hash = (HV*) SvRV(factory);

  FactoryMap::iterator I = _factory_map.find(FactoryMap::key_type(hash, world));
  return (I != _factory_map.end()) ? I->second : 0;
}

using namespace Eris;
using std::string;
using SigCPerl::SignalBase;

MODULE = WorldForge::Eris::World		PACKAGE = WorldForge::Eris::World

void
WorldHandle::DESTROY()

Entity*
World::lookup(string s)
  PREINIT:
    const char* CLASS = "WorldForge::Eris::Entity";

Entity*
World::getRootEntity()
  PREINIT:
    const char* CLASS = "WorldForge::Eris::Entity";

SV*
World::getConnection()
  CODE:
    RETVAL = sv_2mortal(newRV_inc(getControlingSV(connection_hash_string,
	THIS->getConnection())));
  OUTPUT:
    RETVAL

void
World::tick()

Entity*
World::getFocusedEntity()
  PREINIT:
    const char* CLASS = "WorldForge::Eris::Entity";

string
World::getFocusedEntityID()

void
World::registerFactory(SV* factory, unsigned priority = 1)
  CODE:
    if(!sv_derived_from(factory, "WorldForge::Eris::Factory"))
      XSRETURN_UNDEF;
    THIS->registerFactory(new PerlFactory(factory, THIS), priority);

void
World::unregisterFactory(SV* factory)
  CODE:
    if(!sv_derived_from(factory, "WorldForge::Eris::Factory"))
      XSRETURN_UNDEF;
    PerlFactory* factory_inst = PerlFactory::find(factory, THIS);
    if(!factory_inst)
      XSRETURN_UNDEF;
    THIS->unregisterFactory(factory_inst);
    delete factory_inst;

Avatar*
World::getPrimaryAvatar()
  PREINIT:
    const char* CLASS = "WorldForge::Eris::Avatar";

SignalBase*
World::EntityCreate()
  CODE:
    const char *CLASS = "SigC::Signal";
    RETVAL = SigCPerl::WrapSignal(THIS->EntityCreate);
  OUTPUT:
    RETVAL

SignalBase*
World::EntityDelete()
  CODE:
    const char *CLASS = "SigC::Signal";
    RETVAL = SigCPerl::WrapSignal(THIS->EntityDelete);
  OUTPUT:
    RETVAL

SignalBase*
World::Entered()
  CODE:
    const char *CLASS = "SigC::Signal";
    RETVAL = SigCPerl::WrapSignal(THIS->Entered);
  OUTPUT:
    RETVAL

SignalBase*
World::Appearance()
  CODE:
    const char *CLASS = "SigC::Signal";
    RETVAL = SigCPerl::WrapSignal(THIS->Appearance);
  OUTPUT:
    RETVAL

SignalBase*
World::Disappearance()
  CODE:
    const char *CLASS = "SigC::Signal";
    RETVAL = SigCPerl::WrapSignal(THIS->Disappearance);
  OUTPUT:
    RETVAL

SignalBase*
World::RootEntityChanged()
  CODE:
    const char *CLASS = "SigC::Signal";
    RETVAL = SigCPerl::WrapSignal(THIS->RootEntityChanged);
  OUTPUT:
    RETVAL

SignalBase*
World::Destroyed()
  CODE:
    const char *CLASS = "SigC::Signal";
    RETVAL = SigCPerl::WrapSignal(THIS->Destroyed);
  OUTPUT:
    RETVAL
