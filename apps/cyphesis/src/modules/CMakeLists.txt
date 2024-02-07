add_library(cyphesis-modules
        DateTime.cpp
        WorldTime.cpp)


target_link_libraries(cyphesis-modules PUBLIC
        cyphesis-common
)
