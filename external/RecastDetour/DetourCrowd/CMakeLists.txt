add_library(external-DetourCrowd
        Source/DetourCrowd.cpp
        Source/DetourLocalBoundary.cpp
        Source/DetourObstacleAvoidance.cpp
        Source/DetourPathCorridor.cpp
        Source/DetourPathQueue.cpp
        Source/DetourProximityGrid.cpp
)

target_include_directories(external-DetourCrowd PUBLIC Include ../Detour/Include)
target_link_libraries(external-DetourCrowd PUBLIC external-Detour)
