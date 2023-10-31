add_library(cyphesis-navigation
        Awareness.cpp
        fastlz.c
        Steering.cpp
        AwarenessUtils.h
        IHeightProvider.h)

target_link_libraries(cyphesis-navigation PUBLIC
        cyphesis-Detour
        cyphesis-DetourTileCache
        cyphesis-Recast
        cyphesis-remotery
        cyphesis-rulesai)

#target_include_directories(navigation PUBLIC
#    ../../external/RecastDetour/Recast/Include
#    ../../external/RecastDetour/Detour/Include
#    ../../external/RecastDetour/DetourTileCache/Include)

