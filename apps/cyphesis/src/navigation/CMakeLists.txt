add_library(cyphesis-navigation
        Awareness.cpp
        fastlz.c
        Steering.cpp
        AwarenessUtils.h
        IHeightProvider.h
)

target_link_libraries(cyphesis-navigation PUBLIC
        cyphesis-rulesai
        external-Detour
        external-DetourTileCache
        external-Recast
        external-remotery
)
