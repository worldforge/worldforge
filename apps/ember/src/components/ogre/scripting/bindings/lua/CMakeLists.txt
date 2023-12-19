add_subdirectory(ogre)

file(GLOB BINDINGS_FILES *.cpp)

add_library(ember-BindingsEmberOgre STATIC
        ${BINDINGS_FILES}
        helpers/OgreUtils.cpp)

target_link_libraries(ember-BindingsEmberOgre PUBLIC
        ember-ogre
        ember-ogre_terrain
        ember-widgets
        ember-external-sol2
        ember-external-OGRE
)

if (MSVC)
    #Need to allow for a lot of symbols when compiling the bindings on MSVC.
    target_compile_options(ember-BindingsEmberOgre PRIVATE "/bigobj")
endif ()

target_precompile_headers(ember-BindingsEmberOgre
        REUSE_FROM
        ember-BindingsLua)
target_precompile_headers(ember-BindingsEmberOgre
        REUSE_FROM
        ember-ogre)