add_executable(cyphesis-pythondocgenerator
        PythonDocGenerator.cpp)


target_link_libraries(cyphesis-pythondocgenerator PUBLIC
        cyphesis-rulesai_python
        cyphesis-rulessimulation_python
        cyphesis-entityfilter_python
        cyphesis-rulesbase_python
        cyphesis-atlas_python
        cyphesis-common_python
        cyphesis-physics_python
        )
