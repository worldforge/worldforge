add_library(ember-external-pagedgeometry OBJECT
        source/BatchPage.cpp
        source/BatchedGeometry.cpp
        source/GrassLoader.cpp
        source/ImpostorPage.cpp
        source/PagedGeometry.cpp
        source/PropertyMaps.cpp
        source/StaticBillboardSet.cpp
        source/TreeLoader2D.cpp
        source/TreeLoader3D.cpp
        source/DummyPage.cpp
        source/PassiveEntityPage.cpp
        source/WindBatchedGeometry.cpp
        source/WindBatchPage.cpp
        source/ShaderHelper.cpp
)

target_include_directories(ember-external-pagedgeometry PUBLIC include)

target_link_libraries(ember-external-pagedgeometry PUBLIC
        eris
        OgreMain
)

