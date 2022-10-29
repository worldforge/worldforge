add_library(
        core

        core/Record.cpp
        core/Generator.cpp
        core/Iterator.cpp
        core/Provider.cpp
        core/Repository.cpp
        core/Resolver.cpp
        core/Root.cpp
        core/Signature.cpp)

target_include_directories(core PUBLIC
        core)

target_link_libraries(core PUBLIC
        sha256
        CONAN_PKG::spdlog)

add_library(
        curl

        curl/CurlProvider.cpp)

target_include_directories(curl PUBLIC
        curl)

target_link_libraries(curl PUBLIC
        core
        CONAN_PKG::libcurl)

add_executable(
        squall

        cli/CLI.cpp
)
target_include_directories(squall PUBLIC
        cli)

target_link_libraries(squall PUBLIC
        curl
        core
        CONAN_PKG::cli11)