#ifndef ERIS_WORLD_HANDLE_H
#define ERIS_WORLD_HANDLE_H

#include <Eris/World.h>
#include <Eris/Avatar.h>

#include <sigc++/object_slot.h>

#include "refcount.h"

class WorldHandle : public SigC::Object
{
 public:
  WorldHandle(Eris::World* w) : _w(w)
  {
    w->Destroyed.connect(SigC::slot(*this,&WorldHandle::destroy));
    playerRef(w->getPlayer());
  }
  ~WorldHandle() {if(_w) playerUnref(_w->getPlayer());}

  operator Eris::World*() const {return _w;}

 private:
  // set reference invalid
  void destroy() {if(_w) playerUnref(_w->getPlayer()); _w = 0;}

  Eris::World* _w;
};

class AvatarHandle : public SigC::Object
{
 public:
  AvatarHandle(Eris::Avatar* a) : _a(a)
  {
    a->getWorld()->Destroyed.connect(SigC::slot(*this,&AvatarHandle::destroy));
    playerRef(a->getWorld()->getPlayer());
  }
  ~AvatarHandle() {if(_a) playerUnref(_a->getWorld()->getPlayer());}

  operator Eris::Avatar*() const {return _a;}

 private:
  // set reference invalid
  void destroy() {if(_a) playerUnref(_a->getWorld()->getPlayer()); _a = 0;}

  Eris::Avatar* _a;
};

template<>
struct HandleType<Eris::World*>
{
  typedef WorldHandle type;
};

template<>
struct HandleType<Eris::Avatar*>
{
  typedef AvatarHandle type;
};

#endif
