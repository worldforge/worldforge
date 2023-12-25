add_library(ember-ogre-lod STATIC
        LodLevelManager.cpp

        LodDefinition.cpp
        LodDefinitionManager.cpp
        LodManager.cpp
        XMLLodDefinitionSerializer.cpp
        PMInjectorSignaler.cpp
        ScaledPixelCountLodStrategy.cpp

)

target_link_libraries(ember-ogre-lod PUBLIC
        ember-framework
        ember-external-OGRE
        OgreMeshLodGenerator
        OgreMain
)

target_include_directories(ember-ogre-lod PUBLIC .)

