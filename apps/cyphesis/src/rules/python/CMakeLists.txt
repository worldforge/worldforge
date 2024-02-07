add_library(cyphesis-atlas_python
        CyPy_Atlas.cpp
        CyPy_Operation.cpp
        CyPy_Element.cpp
        CyPy_RootEntity.cpp
        CyPy_Oplist.cpp
        CyPy_Root.cpp
)

target_link_libraries(cyphesis-atlas_python PUBLIC
        cyphesis-pythonbase
        Atlas
        AtlasCodecs
        AtlasNet
        AtlasMessage
        AtlasObjects
        AtlasFilters
)

add_library(cyphesis-physics_python
        CyPy_Point3D.cpp
        CyPy_Vector3D.cpp
        CyPy_Quaternion.cpp
        CyPy_Axisbox.cpp
        CyPy_Physics.cpp
        CyPy_Ball.cpp
)

target_link_libraries(cyphesis-physics_python PUBLIC
        cyphesis-atlas_python
        cyphesis-pythonbase
        cyphesis-physics
)

#TODO: move part into the "common" directory and rename what's kept as "cyphesis-rulesbase_python"
add_library(cyphesis-common_python
        CyPy_Common.cpp
)

target_link_libraries(cyphesis-common_python PUBLIC
        cyphesis-physics_python
        cyphesis-pythonbase
        cyphesis-rulesbase
)
