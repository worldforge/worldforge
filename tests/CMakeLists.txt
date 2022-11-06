add_definitions(-DTESTDATADIR="${CMAKE_CURRENT_SOURCE_DIR}/data")

wf_add_test(SignatureTest.cpp)
target_link_libraries(SignatureTest PRIVATE
        squallcore)

wf_add_test(GeneratorTest.cpp)
target_link_libraries(GeneratorTest PRIVATE
        squallcore)

wf_add_test(IteratorTest.cpp)
target_link_libraries(IteratorTest PRIVATE
        squallcore)

wf_add_test(RepositoryTest.cpp)
target_link_libraries(RepositoryTest PRIVATE
        squallcore)

wf_add_test(ProviderTest.cpp)
target_link_libraries(ProviderTest PRIVATE
        squallcore)

wf_add_test(ResolverTest.cpp)
target_link_libraries(ResolverTest PRIVATE
        squallcore)

wf_add_test(CurlProviderTest.cpp)
target_link_libraries(CurlProviderTest PRIVATE
        squallcore
        squallcurl)

wf_add_test(CurlResolverTest.cpp)
target_link_libraries(CurlResolverTest PRIVATE
        squallcore
        squallcurl)

wf_add_test(RealizerTest.cpp)
target_link_libraries(RealizerTest PRIVATE
        squallcore)
