add_library(cyphesis-rulesbase
        Modifier.cpp
)

target_link_libraries(cyphesis-rulesbase PUBLIC
        cyphesis-common
        wfmath
        Atlas
        libsigcpp::sigc++
)


add_subdirectory(python)
add_subdirectory(ai)
add_subdirectory(simulation)




