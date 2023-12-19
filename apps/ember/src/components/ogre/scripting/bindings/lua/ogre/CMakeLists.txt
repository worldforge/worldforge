file(GLOB BINDINGS_FILES *.cpp)

add_library(ember-BindingsOgre STATIC
        ${BINDINGS_FILES}
)
target_link_libraries(ember-BindingsOgre PUBLIC
        ember-framework
        ember-external-sol2
        ember-external-OGRE
        OgreMain
        OgreMeshLodGenerator
)

target_precompile_headers(ember-BindingsOgre
        PUBLIC
        <OGRE/OgreAnyCleaned.h>
        <Ogre.h>
        <sol2/sol.hpp>
)

if (MSVC)
    #Need to allow for a lot of symbols when compiling the bindings on MSVC.
    target_compile_options(ember-BindingsOgre PRIVATE "/bigobj")
endif ()