add_library(ember-Detour OBJECT
        Source/DetourAlloc.cpp
        Source/DetourCommon.cpp
        Source/DetourNavMesh.cpp
        Source/DetourNavMeshBuilder.cpp
        Source/DetourNavMeshQuery.cpp
        Source/DetourNode.cpp
        Source/DetourObstacleAvoidance.cpp
)

target_include_directories(ember-Detour PUBLIC Include)
