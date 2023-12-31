// AUTOGENERATED file, created by the tool generate_stub.py, don't edit!
// If you want to add your own functionality, instead edit the stubPendingPossession_custom.h file.

#ifndef STUB_SERVER_PENDINGPOSSESSION_H
#define STUB_SERVER_PENDINGPOSSESSION_H

#include "server/PendingPossession.h"
#include "stubPendingPossession_custom.h"

#ifndef STUB_PendingPossession_PendingPossession
//#define STUB_PendingPossession_PendingPossession
   PendingPossession::PendingPossession(const std::string &, const std::string &)
  {
    
  }
#endif //STUB_PendingPossession_PendingPossession

#ifndef STUB_PendingPossession_getPossessKey
//#define STUB_PendingPossession_getPossessKey
  const std::string& PendingPossession::getPossessKey() const
  {
    static std::string _static_instance; return _static_instance;
  }
#endif //STUB_PendingPossession_getPossessKey

#ifndef STUB_PendingPossession_getEntityID
//#define STUB_PendingPossession_getEntityID
  const std::string& PendingPossession::getEntityID() const
  {
    static std::string _static_instance; return _static_instance;
  }
#endif //STUB_PendingPossession_getEntityID

#ifndef STUB_PendingPossession_validate
//#define STUB_PendingPossession_validate
  bool PendingPossession::validate(const std::string &, const std::string &) const
  {
    return false;
  }
#endif //STUB_PendingPossession_validate

#ifndef STUB_PendingPossession_setValidated
//#define STUB_PendingPossession_setValidated
  void PendingPossession::setValidated()
  {
    
  }
#endif //STUB_PendingPossession_setValidated

#ifndef STUB_PendingPossession_isValidated
//#define STUB_PendingPossession_isValidated
  bool PendingPossession::isValidated() const
  {
    return false;
  }
#endif //STUB_PendingPossession_isValidated


#endif