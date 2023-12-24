add_library(external-bytesize INTERFACE)

target_include_directories(external-bytesize INTERFACE .)

target_link_libraries(external-bytesize INTERFACE
        fmt::fmt
)


