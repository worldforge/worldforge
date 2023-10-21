set(CORE_SOURCE_FILES
        squall/core/Manifest.cpp
        squall/core/Generator.cpp
        squall/core/Iterator.cpp
        squall/core/Provider.cpp
        squall/core/Realizer.cpp
        squall/core/Repository.cpp
        squall/core/Resolver.cpp
        squall/core/Signature.cpp
        squall/core/AsyncProvider.cpp
        )

set(CORE_HEADER_FILES
        squall/core/Manifest.h
        squall/core/Generator.h
        squall/core/Iterator.h
        squall/core/Provider.h
        squall/core/Realizer.h
        squall/core/Repository.h
        squall/core/Resolver.h
        squall/core/Root.h
        squall/core/Signature.h)


find_package(spdlog REQUIRED)
wf_add_library(squallcore CORE_SOURCE_FILES CORE_HEADER_FILES)
target_link_libraries(squallcore PUBLIC
        sha256
        spdlog::spdlog)


find_package(CURL REQUIRED)
set(CURL_SOURCE_FILES
        squall/curl/CurlProvider.cpp)
set(CURL_HEADER_FILES
        squall/curl/CurlProvider.h)
wf_add_library(squallcurl CURL_SOURCE_FILES CURL_HEADER_FILES)

target_link_libraries(squallcurl PUBLIC
        squallcore
        CURL::libcurl
        )

add_subdirectory(cli)
