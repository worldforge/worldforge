add_library(cyphesis-cyaiclient
        PossessionClient.cpp
        PossessionAccount.cpp
)


target_link_libraries(cyphesis-cyaiclient
        cyphesis-client
        cyphesis-rulesai_python
        cyphesis-rulesai
        cyphesis-navigation
        cyphesis-atlas_python
        cyphesis-common_python
        cyphesis-physics_python
        cyphesis-rulesbase
        cyphesis-modules
        cyphesis-common
        cyphesis-physics
        external-DetourTileCache
        external-Detour
        external-Recast
        external-remotery
)


add_executable(cyaiclient
        aiclient.cpp)

#If Python has been built statically (as with Conan) we need to enable exports so the dynamic loading works.
if (PYTHON_IS_STATIC)
    set_property(TARGET cyaiclient PROPERTY ENABLE_EXPORTS 1)
endif ()

target_link_libraries(cyaiclient
        cyphesis-cyaiclient
)

install(TARGETS cyaiclient DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})
