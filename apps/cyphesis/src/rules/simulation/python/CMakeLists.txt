add_library(cyphesis-rulessimulation_python
        CyPy_Entity.cpp
        CyPy_Server.cpp
        CyPy_Task.cpp
        CyPy_World.cpp
        CyPy_TerrainProperty.cpp
        CyPy_UsageInstance.cpp
        CyPy_EntityProps.cpp
        CyPy_Domain.cpp
        CyPy_LocatedEntity.cpp
)

target_link_libraries(cyphesis-rulessimulation_python PUBLIC
        cyphesis-rulessimulation
        cyphesis-common_python
        cyphesis-pythonbase
)



