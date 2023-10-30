#include <Eris/Player.h>
#include <Eris/Connection.h>
#include <Eris/Factory.h>
#include <Eris/Avatar.h>
#include <Eris/World.h>

#include <sigcperl/signal_wrap.h>

#include "../refcount.h"
#include "../perlentity.h"
#include "../worldhandle.h"
#include "../atlas_convert.h"

// This only gets used if someone emits one of player's signals
// from outside. This should never happen.
namespace SigCPerl {
  template<> inline ArgBox<Eris::LoginFailureType>::ArgBox(SV*) throw()
  {
    assert(false);
  }
}

static SV* GetSV(Eris::LoginFailureType type)
{
  const char *reason;

  switch(type) {
    case Eris::LOGIN_DUPLICATE_ACCOUNT:
      reason = "Duplicate Account";
      break;
    case Eris::LOGIN_BAD_ACCOUNT:
      reason = "Bad Account";
      break;
    case Eris::LOGIN_UNKNOWN_ACCOUNT:
      reason = "Unknown Account";
      break;
    case Eris::LOGIN_BAD_PASSWORD:
      reason = "Bad Password";
      break;
    case Eris::LOGIN_DUPLICATE_CONNECT:
      reason = "Duplicate Connect";
      break;
    default:
      reason = "Unknown Reason";
      break;
  }

  return SigCPerl::GetSV(reason);
}

class CharacterSignalWrap : public SigCPerl::SignalBase
{
 public:
  typedef SigC::Signal1<void, const Atlas::Objects::Entity::GameEntity&> SignalType;
  typedef SigC::Slot1<void, const Atlas::Objects::Entity::GameEntity&> SlotType;

  CharacterSignalWrap(SignalType &signal, SV* parent = 0) throw()
    : m_signal(signal), m_parent(parent) {}

  virtual SigC::Connection connect(const SigCPerl::Slot &slot) throw()
  {
    return m_signal.connect(get_slot(slot));
  }

  static SlotType get_slot(const SigCPerl::Slot &slot) throw()
  {
    return new SigC::AdaptorSlotNode((SigC::FuncPtr) &conv_func, slot.slot());
  }

  virtual SigCPerl::Data emit(const SigCPerl::Data &data, I32 flags)
    throw(SigCPerl::BadConvertVal)
      {assert(false);}

 private:
  static void conv_func(const Atlas::Objects::Entity::GameEntity& entity,
    SigC::AdaptorSlotNode* node)
  {
    Atlas::Message::Object obj = entity.AsObject();

    assert(obj.IsMap());

    const Atlas::Message::Object::MapType &map = obj.AsMap();
    Atlas::Message::Object::MapType::const_iterator I;
    SigCPerl::Data data;

    dSP;
    ENTER;
    SAVETMPS;

    for(I = map.begin(); I != map.end(); ++I) {
      data.push_back(I->first);
      data.push_back(I->second);
    }

    SigC::SlotNode *slot = static_cast<SigC::SlotNode*>(node->slot_.impl());
    ((SigCPerl::Slot::Proxy)(slot->proxy_))(data, G_VOID, slot);

    FREETMPS;
    LEAVE;
  }

  CharacterSignalWrap(const CharacterSignalWrap&);
  CharacterSignalWrap& operator=(const CharacterSignalWrap&);

  SignalType &m_signal;
  SigCPerl::ParentBox m_parent;
};

class DefaultFactory : public Eris::Factory
{
 public:
  virtual bool accept(const Atlas::Objects::Entity::GameEntity &ge, Eris::World *world)
	{return true;}
  virtual Eris::EntityPtr instantiate(const Atlas::Objects::Entity::GameEntity &ge, Eris::World *world)
	{return new PerlEntity(ge, world);}
};

extern "C" {
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
}

using namespace Eris;
using std::string;
using SigCPerl::SignalBase;
typedef Atlas::Message::Object AtlasObject;

// Currently missing getCharacters(), createCharacter(), takeCharacter()

MODULE = WorldForge::Eris::Player		PACKAGE = WorldForge::Eris::Player		

Player*
Player::new(Connection *conn)
  CLEANUP:
    connectionRef(RETVAL->getConnection());
    registerControlingSV(player_hash_string, RETVAL, SvRV(ST(0)));


void
Player::DESTROY()
  INIT:
    connectionUnref(THIS->getConnection());
    unregisterControlingSV(player_hash_string, THIS);


void
Player::login(string uname, string pwd)

void
Player::createAccount(string uname, string name, string pwd)

void
Player::logout()

void
Player::getCharacters()
  PPCODE:
    CharacterList characters = THIS->getCharacters();
    CharacterList::const_iterator I;
    for(I = characters.begin(); I != characters.end(); ++I)
      XPUSHs(sv_2mortal(AtlasToSV(I->AsObject())));

void
Player::refreshCharacterInfo()

Avatar*
Player::takeCharacter(string id)
  PREINIT:
    const char *CLASS = "WorldForge::Eris::Avatar";
  CLEANUP:
    RETVAL->getWorld()->registerFactory(new DefaultFactory(), 0);

Avatar*
Player::_createCharacter(AtlasObject attrs)
  CODE:
    const char *CLASS = "WorldForge::Eris::Avatar";
    Atlas::Objects::Entity::GameEntity character
      = Atlas::Objects::Entity::GameEntity::Instantiate();

    assert(attrs.IsMap());
    const AtlasObject::MapType &map = attrs.AsMap();
    AtlasObject::MapType::const_iterator I;
    for(I = map.begin(); I != map.end(); ++I)
      character.SetAttr(I->first, I->second);

    RETVAL = THIS->createCharacter(character);

    RETVAL->getWorld()->registerFactory(new DefaultFactory(), 0);
  OUTPUT:
    RETVAL

string
Player::getAccountID()

SignalBase*
Player::GotCharacterInfo()
  CODE:
    const char *CLASS = "SigC::Signal";
    RETVAL = new CharacterSignalWrap(THIS->GotCharacterInfo, ST(0));
  OUTPUT:
    RETVAL

SignalBase*
Player::GotAllCharacters()
  CODE:
    const char *CLASS = "SigC::Signal";
    RETVAL = SigCPerl::WrapSignal(THIS->GotAllCharacters, ST(0));
  OUTPUT:
    RETVAL

SignalBase*
Player::LoginFailure()
  CODE:
    const char *CLASS = "SigC::Signal";
    RETVAL = SigCPerl::WrapSignal(THIS->LoginFailure, ST(0));
  OUTPUT:
    RETVAL

SignalBase*
Player::LoginSuccess()
  CODE:
    const char *CLASS = "SigC::Signal";
    RETVAL = SigCPerl::WrapSignal(THIS->LoginSuccess, ST(0));
  OUTPUT:
    RETVAL

SignalBase*
Player::LogoutComplete()
  CODE:
    const char *CLASS = "SigC::Signal";
    RETVAL = SigCPerl::WrapSignal(THIS->LogoutComplete, ST(0));
  OUTPUT:
    RETVAL
