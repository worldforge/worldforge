add_compile_definitions("TESTDATADIR=\"${CMAKE_CURRENT_SOURCE_DIR}/data\"")

wf_add_test(SignatureTest.cpp)
target_link_libraries(SignatureTest
        squallcore)

wf_add_test(GeneratorTest.cpp)
target_link_libraries(GeneratorTest
        squallcore)

wf_add_test(IteratorTest.cpp)
target_link_libraries(IteratorTest
        squallcore)

wf_add_test(RepositoryTest.cpp)
target_link_libraries(RepositoryTest
        squallcore)

wf_add_test(ProviderTest.cpp)
target_link_libraries(ProviderTest
        squallcore)

wf_add_test(ResolverTest.cpp)
target_link_libraries(ResolverTest
        squallcore)

wf_add_test(CurlProviderTest.cpp)
target_link_libraries(CurlProviderTest
        squallcore
        squallcurl)

wf_add_test(CurlResolverTest.cpp)
target_link_libraries(CurlResolverTest
        squallcore
        squallcurl)

wf_add_test(RealizerTest.cpp)
target_link_libraries(RealizerTest
        squallcore)

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