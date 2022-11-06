set(CORE_SOURCE_FILES
        core/Record.cpp
        core/Generator.cpp
        core/Iterator.cpp
        core/Provider.cpp
        core/Realizer.cpp
        core/Repository.cpp
        core/Resolver.cpp
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


wf_add_library(squallcore CORE_SOURCE_FILES CORE_HEADER_FILES)
target_include_directories(squallcore PUBLIC
        core)
target_link_libraries(squallcore PUBLIC
        sha256
        spdlog)


find_package(CURL 7 REQUIRED)
set(CURL_SOURCE_FILES
        curl/CurlProvider.cpp)
set(CURL_HEADER_FILES
        curl/CurlProvider.h)
wf_add_library(squallcurl CURL_SOURCE_FILES CURL_HEADER_FILES)
target_include_directories(squallcurl PUBLIC
        curl)
target_link_libraries(squallcurl PUBLIC
        squallcore
        ${CURL_LIBRARIES}
        )


add_executable(
        squall
        cli/CLI.cpp
)
target_include_directories(squall PUBLIC
        cli)
target_link_libraries(squall
        squallcurl
        squallcore
        )
if (EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    conan_target_link_libraries(squall)
endif ()

INSTALL(TARGETS squall)