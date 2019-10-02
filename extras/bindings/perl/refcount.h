#ifndef ERIS_PERL_REFCOUNT_H
#define ERIS_PERL_REFCOUNT_H

#include <sigcperl/convert.h>

extern "C" {
#include "EXTERN.h"
#include "perl.h"
}
#undef convert
#undef list
#undef ref
#undef scalar

template<class C> struct HandleType;

namespace Eris {class Connection; class Player;}

// find the SV that owns a particular instance
// of a C++ class using a named hash
SV* getControlingSV(const char* hash, void* ptr)
{
  HV* hv = get_hv(hash, 0);
  assert(hv);

  SV* key = newSViv((I32) ptr);
  STRLEN len;
  const char* str = SvPV(key, len);

  SV** result = hv_fetch(hv, str, len, 0);
  SvREFCNT_dec(key);
  assert(result);
  return *result;
}

void registerControlingSV(const char* hash, void* ptr, SV* sv)
{
  HV* hv = get_hv(hash, 0);
  assert(hv);

  SV* key = newSViv((I32) ptr);
  STRLEN len;
  const char* str = SvPV(key, len);

  hv_store(hv, str, len, sv, 0);
  SvREFCNT_dec(key);
}

void unregisterControlingSV(const char* hash, void* ptr)
{
  HV* hv = get_hv(hash, 0);
  assert(hv);

  SV* key = newSViv((I32) ptr);
  STRLEN len;
  const char* str = SvPV(key, len);

  hv_delete(hv, str, len, 0);
  SvREFCNT_dec(key);
}

const char connection_hash_string[] = "WorldForge::Eris::Connection::_Refcount_Hash_";
const char player_hash_string[] = "WorldForge::Eris::Player::_Refcount_Hash_";

inline void connectionRef(Eris::Connection* con)
{
  SvREFCNT_inc(getControlingSV(connection_hash_string, con));
}

inline void connectionUnref(Eris::Connection* con)
{
  SvREFCNT_dec(getControlingSV(connection_hash_string, con));
}

inline void playerRef(Eris::Player* player)
{
  SvREFCNT_inc(getControlingSV(player_hash_string, player));
}

inline void playerUnref(Eris::Player* player)
{
  SvREFCNT_dec(getControlingSV(player_hash_string, player));
}

#endif
