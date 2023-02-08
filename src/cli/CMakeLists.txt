find_package(CLI11 REQUIRED)
add_executable(squall-bin
        CLI.cpp
        )
set_target_properties(squall-bin
        PROPERTIES OUTPUT_NAME squall)
target_link_libraries(squall-bin
        squallcurl
        squallcore
        CLI11::CLI11
        )

INSTALL(TARGETS squall-bin)