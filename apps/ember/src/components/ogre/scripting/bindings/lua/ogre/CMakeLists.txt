file(GLOB BINDINGS_FILES *.cpp)

add_library(ember-BindingsOgre OBJECT
        ${BINDINGS_FILES}
)
target_link_libraries(ember-BindingsOgre PUBLIC
        ember-external-sol2
        ember-external-OGRE
)

target_precompile_headers(ember-BindingsOgre
        REUSE_FROM
        ember-BindingsLua)
target_precompile_headers(ember-BindingsOgre
        REUSE_FROM
        ember-ogre)

if (MSVC)
    #Need to allow for a lot of symbols when compiling the bindings on MSVC.
    target_compile_options(ember-BindingsOgre PRIVATE "/bigobj")
endif ()