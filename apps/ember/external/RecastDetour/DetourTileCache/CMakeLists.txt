add_library(ember-DetourTileCache OBJECT
        Source/DetourTileCache.cpp
        Source/DetourTileCacheBuilder.cpp
)

target_link_libraries(ember-DetourTileCache PUBLIC
        ember-Detour
)

target_include_directories(ember-DetourTileCache PUBLIC Include)
