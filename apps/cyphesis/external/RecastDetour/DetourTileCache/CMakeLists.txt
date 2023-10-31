add_library(cyphesis-DetourTileCache
        Source/DetourTileCache.cpp
        Source/DetourTileCacheBuilder.cpp)

target_include_directories(cyphesis-DetourTileCache PUBLIC Include ../Detour/Include)
target_link_libraries(cyphesis-DetourTileCache PUBLIC cyphesis-Detour)
