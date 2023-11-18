//Add custom implementations of stubbed functions here; this file won't be rewritten when re-generating stubs.

#include "common/FileSystemObserver.h"

#ifndef STUB_AssetsManager_AssetsManager
#define STUB_AssetsManager_AssetsManager
AssetsManager::AssetsManager(std::unique_ptr<FileSystemObserver> file_system_observer)
    : Singleton(), m_file_system_observer(std::move(file_system_observer))
{

}
#endif //STUB_AssetsManager_AssetsManager
