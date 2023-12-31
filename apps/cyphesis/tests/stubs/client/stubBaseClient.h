// AUTOGENERATED file, created by the tool generate_stub.py, don't edit!
// If you want to add your own functionality, instead edit the stubBaseClient_custom.h file.

#ifndef STUB_CLIENT_BASECLIENT_H
#define STUB_CLIENT_BASECLIENT_H

#include "client/BaseClient.h"
#include "stubBaseClient_custom.h"

#ifndef STUB_BaseClient_getErrorMessage
//#define STUB_BaseClient_getErrorMessage
   std::string BaseClient::getErrorMessage(const Operation& err)
  {
    return "";
  }
#endif //STUB_BaseClient_getErrorMessage

#ifndef STUB_BaseClient_notifyAccountCreated
//#define STUB_BaseClient_notifyAccountCreated
  void BaseClient::notifyAccountCreated(RouterId accountId)
  {
    
  }
#endif //STUB_BaseClient_notifyAccountCreated

#ifndef STUB_BaseClient_BaseClient
//#define STUB_BaseClient_BaseClient
   BaseClient::BaseClient(CommSocket& commSocket)
    : Link(commSocket)
  {
    
  }
#endif //STUB_BaseClient_BaseClient

#ifndef STUB_BaseClient_createSystemAccount
//#define STUB_BaseClient_createSystemAccount
  void BaseClient::createSystemAccount(const std::string& usernameSuffix )
  {
    
  }
#endif //STUB_BaseClient_createSystemAccount

#ifndef STUB_BaseClient_createAccount
//#define STUB_BaseClient_createAccount
  Atlas::Objects::Root BaseClient::createAccount(const std::string& name, const std::string& pword)
  {
    return *static_cast<Atlas::Objects::Root*>(nullptr);
  }
#endif //STUB_BaseClient_createAccount

#ifndef STUB_BaseClient_createCharacter
//#define STUB_BaseClient_createCharacter
  CreatorClient* BaseClient::createCharacter(const std::string& name)
  {
    return nullptr;
  }
#endif //STUB_BaseClient_createCharacter

#ifndef STUB_BaseClient_logout
//#define STUB_BaseClient_logout
  void BaseClient::logout()
  {
    
  }
#endif //STUB_BaseClient_logout

#ifndef STUB_BaseClient_externalOperation
//#define STUB_BaseClient_externalOperation
  void BaseClient::externalOperation(const Operation& op, Link&)
  {
    
  }
#endif //STUB_BaseClient_externalOperation

#ifndef STUB_BaseClient_runTask
//#define STUB_BaseClient_runTask
  int BaseClient::runTask(std::shared_ptr<ClientTask> task, const std::string& arg)
  {
    return 0;
  }
#endif //STUB_BaseClient_runTask

#ifndef STUB_BaseClient_runTask
//#define STUB_BaseClient_runTask
  int BaseClient::runTask(std::function<bool(const Operation&, OpVector&)> function)
  {
    return 0;
  }
#endif //STUB_BaseClient_runTask

#ifndef STUB_BaseClient_endTask
//#define STUB_BaseClient_endTask
  int BaseClient::endTask()
  {
    return 0;
  }
#endif //STUB_BaseClient_endTask

#ifndef STUB_BaseClient_sendWithCallback
//#define STUB_BaseClient_sendWithCallback
  void BaseClient::sendWithCallback(Operation op, std::function<void(const Operation&, OpVector&)> callback, std::function<void()> timeoutCallback )
  {
    
  }
#endif //STUB_BaseClient_sendWithCallback

#ifndef STUB_BaseClient_hasTask
//#define STUB_BaseClient_hasTask
  bool BaseClient::hasTask() const
  {
    return false;
  }
#endif //STUB_BaseClient_hasTask

#ifndef STUB_BaseClient_notifyConnectionComplete
//#define STUB_BaseClient_notifyConnectionComplete
  void BaseClient::notifyConnectionComplete()
  {
    
  }
#endif //STUB_BaseClient_notifyConnectionComplete


#endif