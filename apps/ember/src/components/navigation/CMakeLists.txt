add_library(ember-navigation OBJECT
        Awareness.cpp
        fastlz.c
        Steering.cpp
        Loitering.cpp
        AwarenessUtils.h)

target_link_libraries(ember-navigation PUBLIC
        ember-Detour
        ember-DetourTileCache
        external-Detour
        external-Recast
        external-DetourTileCache
        eris
        wfmath
)

#target_include_directories(navigation PUBLIC
#        ${CMAKE_SOURCE_DIR}/external/RecastDetour/DetourTileCache/Include
#        ${CMAKE_SOURCE_DIR}/external/RecastDetour/Detour/Include)
