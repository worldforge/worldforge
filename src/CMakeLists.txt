add_library(
        core

        core/Record.cpp
        core/Generator.cpp
        core/Iterator.cpp
        core/Provider.cpp
        core/Repository.cpp
        core/Resolver.cpp
        core/Root.cpp)

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