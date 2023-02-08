set(CORE_SOURCE_FILES
        core/Record.cpp
        core/Generator.cpp
        core/Iterator.cpp
        core/Provider.cpp
        core/Realizer.cpp
        core/Repository.cpp
        core/Resolver.cpp
        core/Signature.cpp
        )

set(CORE_HEADER_FILES
        core/Record.h
        core/Generator.h
        core/Iterator.h
        core/Provider.h
        core/Realizer.h
        core/Repository.h
        core/Resolver.h
        core/Root.h
        core/Signature.h)


find_package(spdlog REQUIRED)
wf_add_library(squallcore CORE_SOURCE_FILES CORE_HEADER_FILES)
target_link_libraries(squallcore PUBLIC
        sha256
        spdlog::spdlog)


find_package(CURL 7 REQUIRED)
set(CURL_SOURCE_FILES
        curl/CurlProvider.cpp)
set(CURL_HEADER_FILES
        curl/CurlProvider.h)
wf_add_library(squallcurl CURL_SOURCE_FILES CURL_HEADER_FILES)

target_link_libraries(squallcurl PUBLIC
        squallcore
        CURL::libcurl
        )


find_package(CLI11 REQUIRED)
add_executable(
        squall
        cli/CLI.cpp
)
target_link_libraries(squall
        squallcurl
        squallcore
        CLI11::CLI11
        )

INSTALL(TARGETS squall)