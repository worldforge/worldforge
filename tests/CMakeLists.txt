add_definitions(-DTESTDATADIR="${CMAKE_CURRENT_SOURCE_DIR}/data")

wf_add_test(SignatureTest.cpp)
target_link_libraries(SignatureTest
        squallcore)
if (EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    conan_target_link_libraries(SignatureTest)
endif ()

wf_add_test(GeneratorTest.cpp)
target_link_libraries(GeneratorTest
        squallcore)
if (EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    conan_target_link_libraries(GeneratorTest)
endif ()

wf_add_test(IteratorTest.cpp)
target_link_libraries(IteratorTest
        squallcore)
if (EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    conan_target_link_libraries(IteratorTest)
endif ()

wf_add_test(RepositoryTest.cpp)
target_link_libraries(RepositoryTest
        squallcore)
if (EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    conan_target_link_libraries(RepositoryTest)
endif ()

wf_add_test(ProviderTest.cpp)
target_link_libraries(ProviderTest
        squallcore)
if (EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    conan_target_link_libraries(ProviderTest)
endif ()

wf_add_test(ResolverTest.cpp)
target_link_libraries(ResolverTest
        squallcore)
if (EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    conan_target_link_libraries(ResolverTest)
endif ()

wf_add_test(CurlProviderTest.cpp)
target_link_libraries(CurlProviderTest
        squallcore
        squallcurl)
if (EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    conan_target_link_libraries(CurlProviderTest)
endif ()

wf_add_test(CurlResolverTest.cpp)
target_link_libraries(CurlResolverTest
        squallcore
        squallcurl)
if (EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    conan_target_link_libraries(CurlResolverTest)
endif ()

wf_add_test(RealizerTest.cpp)
target_link_libraries(RealizerTest
        squallcore)
if (EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    conan_target_link_libraries(RealizerTest)
endif ()

add_custom_target(check
        COMMAND RealizerTest
        COMMAND SignatureTest
        COMMAND GeneratorTest
        COMMAND IteratorTest
        COMMAND ProviderTest
        COMMAND RepositoryTest
        COMMAND ResolverTest
        COMMAND CurlProviderTest
        COMMAND CurlResolverTest
        )