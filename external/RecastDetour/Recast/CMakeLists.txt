add_library(external-Recast
        Source/Recast.cpp
        Source/RecastAlloc.cpp
        Source/RecastArea.cpp
        Source/RecastAssert.cpp
        Source/RecastContour.cpp
        Source/RecastFilter.cpp
        Source/RecastLayers.cpp
        Source/RecastMesh.cpp
        Source/RecastMeshDetail.cpp
        Source/RecastRasterization.cpp
        Source/RecastRegion.cpp)

target_include_directories(external-Recast PUBLIC Include)
