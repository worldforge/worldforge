add_library(ember-Recast OBJECT
        Source/Recast.cpp
        Source/RecastArea.cpp
        Source/RecastAlloc.cpp
        Source/RecastContour.cpp
        Source/RecastFilter.cpp
        Source/RecastLayers.cpp
        Source/RecastMesh.cpp
        Source/RecastMeshDetail.cpp
        Source/RecastRasterization.cpp
        Source/RecastRegion.cpp)

target_include_directories(ember-Recast PUBLIC Include)
