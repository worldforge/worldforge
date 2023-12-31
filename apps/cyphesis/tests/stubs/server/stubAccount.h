// AUTOGENERATED file, created by the tool generate_stub.py, don't edit!
// If you want to add your own functionality, instead edit the stubAccount_custom.h file.

#ifndef STUB_SERVER_ACCOUNT_H
#define STUB_SERVER_ACCOUNT_H

#include "server/Account.h"
#include "stubAccount_custom.h"

#ifndef STUB_Account_characterDestroyed
//#define STUB_Account_characterDestroyed
  void Account::characterDestroyed(long)
  {
    
  }
#endif //STUB_Account_characterDestroyed

#ifndef STUB_Account_processExternalOperation
//#define STUB_Account_processExternalOperation
  void Account::processExternalOperation(const Operation& op, OpVector& res)
  {
    
  }
#endif //STUB_Account_processExternalOperation

#ifndef STUB_Account_createMind
//#define STUB_Account_createMind
  std::unique_ptr<ExternalMind> Account::createMind(const Ref<LocatedEntity>& entity) const
  {
    return *static_cast<std::unique_ptr<ExternalMind>*>(nullptr);
  }
#endif //STUB_Account_createMind

#ifndef STUB_Account_removeMindFromEntity
//#define STUB_Account_removeMindFromEntity
  void Account::removeMindFromEntity(ExternalMind* mind)
  {
    
  }
#endif //STUB_Account_removeMindFromEntity

#ifndef STUB_Account_connectCharacter
//#define STUB_Account_connectCharacter
  int Account::connectCharacter(const Ref<LocatedEntity>& entity, OpVector& res)
  {
    return 0;
  }
#endif //STUB_Account_connectCharacter

#ifndef STUB_Account_Account
//#define STUB_Account_Account
   Account::Account(Connection* conn, std::string username, std::string passwd, RouterId id)
    : ConnectableRouter(conn, username, passwd, id)
    , m_connection(nullptr)
  {
    
  }
#endif //STUB_Account_Account

#ifndef STUB_Account_Account_DTOR
//#define STUB_Account_Account_DTOR
   Account::~Account()
  {
    
  }
#endif //STUB_Account_Account_DTOR

#ifndef STUB_Account_getType
//#define STUB_Account_getType
  const char* Account::getType() const
  {
    return nullptr;
  }
#endif //STUB_Account_getType

#ifndef STUB_Account_store
//#define STUB_Account_store
  void Account::store() const
  {
    
  }
#endif //STUB_Account_store

#ifndef STUB_Account_isPersisted
//#define STUB_Account_isPersisted
  bool Account::isPersisted() const
  {
    return false;
  }
#endif //STUB_Account_isPersisted

#ifndef STUB_Account_addToMessage
//#define STUB_Account_addToMessage
  void Account::addToMessage(Atlas::Message::MapType&) const
  {
    
  }
#endif //STUB_Account_addToMessage

#ifndef STUB_Account_addToEntity
//#define STUB_Account_addToEntity
  void Account::addToEntity(const Atlas::Objects::Entity::RootEntity&) const
  {
    
  }
#endif //STUB_Account_addToEntity

#ifndef STUB_Account_externalOperation
//#define STUB_Account_externalOperation
  void Account::externalOperation(const Operation& op, Link&)
  {
    
  }
#endif //STUB_Account_externalOperation

#ifndef STUB_Account_operation
//#define STUB_Account_operation
  void Account::operation(const Operation&, OpVector&)
  {
    
  }
#endif //STUB_Account_operation

#ifndef STUB_Account_LogoutOperation
//#define STUB_Account_LogoutOperation
  void Account::LogoutOperation(const Operation&, OpVector&)
  {
    
  }
#endif //STUB_Account_LogoutOperation

#ifndef STUB_Account_CreateOperation
//#define STUB_Account_CreateOperation
  void Account::CreateOperation(const Operation&, OpVector&)
  {
    
  }
#endif //STUB_Account_CreateOperation

#ifndef STUB_Account_SetOperation
//#define STUB_Account_SetOperation
  void Account::SetOperation(const Operation&, OpVector&)
  {
    
  }
#endif //STUB_Account_SetOperation

#ifndef STUB_Account_ImaginaryOperation
//#define STUB_Account_ImaginaryOperation
  void Account::ImaginaryOperation(const Operation&, OpVector&)
  {
    
  }
#endif //STUB_Account_ImaginaryOperation

#ifndef STUB_Account_TalkOperation
//#define STUB_Account_TalkOperation
  void Account::TalkOperation(const Operation&, OpVector&)
  {
    
  }
#endif //STUB_Account_TalkOperation

#ifndef STUB_Account_LookOperation
//#define STUB_Account_LookOperation
  void Account::LookOperation(const Operation&, OpVector&)
  {
    
  }
#endif //STUB_Account_LookOperation

#ifndef STUB_Account_GetOperation
//#define STUB_Account_GetOperation
  void Account::GetOperation(const Operation&, OpVector&)
  {
    
  }
#endif //STUB_Account_GetOperation

#ifndef STUB_Account_OtherOperation
//#define STUB_Account_OtherOperation
  void Account::OtherOperation(const Operation&, OpVector&)
  {
    
  }
#endif //STUB_Account_OtherOperation

#ifndef STUB_Account_PossessOperation
//#define STUB_Account_PossessOperation
  void Account::PossessOperation(const Operation&, OpVector&)
  {
    
  }
#endif //STUB_Account_PossessOperation

#ifndef STUB_Account_addCharacter
//#define STUB_Account_addCharacter
  void Account::addCharacter(const Ref<LocatedEntity>&)
  {
    
  }
#endif //STUB_Account_addCharacter

#ifndef STUB_Account_sendUpdateToClient
//#define STUB_Account_sendUpdateToClient
  void Account::sendUpdateToClient()
  {
    
  }
#endif //STUB_Account_sendUpdateToClient

#ifndef STUB_Account_setConnection
//#define STUB_Account_setConnection
  void Account::setConnection(Connection* connection)
  {
    
  }
#endif //STUB_Account_setConnection

#ifndef STUB_Account_getConnection
//#define STUB_Account_getConnection
  Connection* Account::getConnection() const
  {
    return nullptr;
  }
#endif //STUB_Account_getConnection


#endif