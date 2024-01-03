add_compile_definitions("TESTDATADIR=\"${CMAKE_CURRENT_SOURCE_DIR}/data\"")

squall_add_test(SignatureTest.cpp)
target_link_libraries(SignatureTest
        squallcore)

squall_add_test(GeneratorTest.cpp)
target_link_libraries(GeneratorTest
        squallcore)

squall_add_test(IteratorTest.cpp)
target_link_libraries(IteratorTest
        squallcore)

squall_add_test(RepositoryTest.cpp)
target_link_libraries(RepositoryTest
        squallcore)

squall_add_test(ProviderTest.cpp)
target_link_libraries(ProviderTest
        squallcore)

squall_add_test(ResolverTest.cpp)
target_link_libraries(ResolverTest
        squallcore)

squall_add_test(CurlProviderTest.cpp)
target_link_libraries(CurlProviderTest
        squallcore
        squallcurl)

squall_add_test(CurlResolverTest.cpp)
target_link_libraries(CurlResolverTest
        squallcore
        squallcurl)

squall_add_test(RealizerTest.cpp)
target_link_libraries(RealizerTest
        squallcore)

squall_add_test(DifferenceTest.cpp)
target_link_libraries(DifferenceTest
        squallcore)

squall_add_test(PrunerTest.cpp)
target_link_libraries(PrunerTest
        squallcore)

add_custom_target(check-squall
        COMMAND PrunerTest
        COMMAND RealizerTest
        COMMAND SignatureTest
        COMMAND GeneratorTest
        COMMAND IteratorTest
        COMMAND ProviderTest
        COMMAND RepositoryTest
        COMMAND ResolverTest
        COMMAND CurlProviderTest
        COMMAND CurlResolverTest
        COMMAND DifferenceTest
)
if (NOT TARGET check)
    add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND})
endif ()

add_dependencies(check check-squall)

