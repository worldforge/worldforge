add_library(ember-external-meshtree OBJECT
        meshtree/MeshTree.cpp
        meshtree/TParameters.cpp
        meshtree/TStem.cpp
)

target_include_directories(ember-external-meshtree PUBLIC .)

target_link_libraries(ember-external-meshtree PUBLIC
        OgreMain
)
