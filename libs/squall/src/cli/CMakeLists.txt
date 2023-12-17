find_package(CLI11 REQUIRED)
add_executable(squall-bin
        CLI.cpp
)
set_target_properties(squall-bin
        PROPERTIES OUTPUT_NAME squall)
target_link_libraries(squall-bin
        external-bytesize
        squallcurl
        squallcore
        CLI11::CLI11
)
set_target_properties(squall-bin
        PROPERTIES INSTALL_RPATH "\\\$ORIGIN/../lib64")

INSTALL(TARGETS squall-bin)