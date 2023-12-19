add_library(ember-external-OGRE INTERFACE)

target_include_directories(ember-external-OGRE INTERFACE .)
target_link_libraries(ember-external-OGRE INTERFACE
        OgreMain
)
