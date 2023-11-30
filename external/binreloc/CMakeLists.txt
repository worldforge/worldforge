add_library(external-binreloc OBJECT
        binreloc.cpp
)

target_include_directories(external-binreloc PUBLIC .)
