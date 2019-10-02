#ifndef ERIS_ROOM_HANDLE_H
#define ERIS_ROOM_HANDLE_H

#include <string>

#include "refcount.h"

#include <Eris/Room.h>
#include <Eris/Lobby.h>

// a class to pass to perl as Eris::Room
class RoomHandle
{
 public:
  RoomHandle(Eris::Lobby* l) : lobby_(l), id_(l->getID()),
	is_lobby_(true) {connectionRef(lobby_->getConnection());}
  RoomHandle(Eris::Room* r) : lobby_(r->getLobby()), id_(r->getID()),
	is_lobby_(r == lobby_) {connectionRef(lobby_->getConnection());}
  ~RoomHandle() {connectionUnref(lobby_->getConnection());}

  // only called in lobby typemap conversion
  operator Eris::Lobby*() {assert(is_lobby_); return lobby_;}
  // may return 0 if Room id is stale
  operator Eris::Room*() {return is_lobby_ ? lobby_ : lobby_->getRoom(id_);}

  const std::string& id() const {return id_;}

 private:
  Eris::Lobby* lobby_;
  std::string id_;
  bool is_lobby_;
};

template<>
struct HandleType<Eris::Room*>
{
  typedef RoomHandle type;
};

template<>
struct HandleType<Eris::Lobby*>
{
  typedef RoomHandle type;
};

#endif
