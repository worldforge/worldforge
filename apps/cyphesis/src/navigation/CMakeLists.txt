add_library(cyphesis-navigation
        Awareness.cpp
        fastlz.c
        Steering.cpp
        AwarenessUtils.h
        IHeightProvider.h
)

target_link_libraries(cyphesis-navigation PUBLIC
        cyphesis-rulesai
        external-DetourCrowd
        external-DetourTileCache
        external-Detour
        external-Recast
        external-remotery
)
