add_library(
        core

        core/Digest.cpp
        core/Generator.cpp
        core/Provider.cpp
        core/Repository.cpp)

target_include_directories(core PUBLIC
        core
        )

target_link_libraries(core PUBLIC
        sha256
        CONAN_PKG::spdlog)