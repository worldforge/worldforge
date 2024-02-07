add_library(cyphesis-rulesai
        BaseMind.cpp
        MemEntity.cpp
        MemMap.cpp
        AwareMind.cpp
        AwareMindFactory.cpp
        AwarenessStore.cpp
        AwarenessStoreProvider.cpp
        SharedTerrain.cpp
        TypeResolver.cpp
)

target_link_libraries(cyphesis-rulesai PUBLIC
        external-remotery
        cyphesis-navigation
        cyphesis-physics
        cyphesis-rulesbase
        mercator
)

add_subdirectory(python)

