#ifndef ERIS_TYPE_INFO_HANDLE_H
#define ERIS_TYPE_INFO_HANDLE_H

#include <string>

#include "refcount.h"

#include <Eris/Entity.h>
#include <Eris/World.h>

namespace Eris {class TypeInfo;}

// a class to pass to perl as Eris::Room
class TypeInfoHandle
{
 public:
  TypeInfoHandle(Eris::Entity* e) : info_(e->getType()),
	con_(e->getWorld()->getConnection()) {connectionRef(con_);}
  ~TypeInfoHandle() {connectionUnref(con_);}

  // only called in lobby typemap conversion
  operator Eris::TypeInfo*() {return info_;}

 private:
  Eris::TypeInfo* info_;
  Eris::Connection* con_;
};

template<>
struct HandleType<Eris::TypeInfo*>
{
  typedef TypeInfoHandle type;
};

#endif
