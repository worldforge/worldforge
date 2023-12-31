// AUTOGENERATED file, created by the tool generate_stub.py, don't edit!
// If you want to add your own functionality, instead edit the stubAssetsHandler_custom.h file.

#ifndef STUB_SERVER_ASSETSHANDLER_H
#define STUB_SERVER_ASSETSHANDLER_H

#include "server/AssetsHandler.h"
#include "stubAssetsHandler_custom.h"

#ifndef STUB_AssetsHandler_AssetsHandler
//#define STUB_AssetsHandler_AssetsHandler
   AssetsHandler::AssetsHandler(std::filesystem::path squallRepositoryPath)
  {
    
  }
#endif //STUB_AssetsHandler_AssetsHandler

#ifndef STUB_AssetsHandler_resolveAssetsUrl
//#define STUB_AssetsHandler_resolveAssetsUrl
  std::string AssetsHandler::resolveAssetsUrl() const
  {
    return "";
  }
#endif //STUB_AssetsHandler_resolveAssetsUrl

#ifndef STUB_AssetsHandler_refreshSquallRepository
//#define STUB_AssetsHandler_refreshSquallRepository
  std::optional<Squall::Signature> AssetsHandler::refreshSquallRepository(std::filesystem::path pathToAssets)
  {
    return *static_cast<std::optional<Squall::Signature>*>(nullptr);
  }
#endif //STUB_AssetsHandler_refreshSquallRepository


#endif