add_library(cyphesis-entityfilter_python
        CyPy_EntityFilter.cpp
)

target_link_libraries(cyphesis-entityfilter_python PUBLIC
        cyphesis-pythonbase
)



