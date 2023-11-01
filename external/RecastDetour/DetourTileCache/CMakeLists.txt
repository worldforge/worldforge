add_library(external-DetourTileCache
        Source/DetourTileCache.cpp
        Source/DetourTileCacheBuilder.cpp)

target_include_directories(external-DetourTileCache PUBLIC Include ../Detour/Include)
target_link_libraries(external-DetourTileCache PUBLIC external-Detour)
