#ifndef ERIS_PERL_ENTITY_H
#define ERIS_PERL_ENTITY_H

#include <Eris/Entity.h>

#include "refcount.h"

class PerlEntity : public Eris::Entity
{
 public:
  PerlEntity(const Atlas::Objects::Entity::GameEntity &ge, Eris::World *world)
	: Eris::Entity(ge, world), _sv(NEWSV(0, 0))
  {
    sv_setref_pv(_sv, "WorldForge::Eris::Entity", this);
  }
  virtual ~PerlEntity() {SvREFCNT_dec(_sv);}

  SV* sv() const {return sv_2mortal(newSVsv(_sv));}

  static PerlEntity* get(SV* sv)
  {
    return (sv_isobject(sv) || SvTYPE(SvRV(sv)) == SVt_PVMG)
	? (PerlEntity*) SvIV((SV*)SvRV(sv)) : 0;
  }

 private:
  SV* _sv;
};

#endif
