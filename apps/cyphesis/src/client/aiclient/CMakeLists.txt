
add_executable(cyaiclient
        PossessionClient.cpp
        aiclient.cpp
        PossessionAccount.cpp)

#If Python has been built statically (as with Conan) we need to enable exports so the dynamic loading works.
if (PYTHON_IS_STATIC)
    set_property(TARGET cyaiclient PROPERTY ENABLE_EXPORTS 1)
endif ()

target_link_libraries(cyaiclient
        cyphesis-client
        cyphesis-rulesai_python
        cyphesis-rulesai
        cyphesis-entityfilter_python
        cyphesis-entityfilter
        cyphesis-navigation
        cyphesis-rulesbase_python
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

install(TARGETS cyaiclient DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})
