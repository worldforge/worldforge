add_definitions(-DTESTDATADIR="${CMAKE_CURRENT_SOURCE_DIR}/data")

add_executable(SignatureTest SignatureTest.cpp)
target_link_libraries(SignatureTest PRIVATE
        CONAN_PKG::catch2
        core)

add_executable(GeneratorTest GeneratorTest.cpp)
target_link_libraries(GeneratorTest PRIVATE
        CONAN_PKG::catch2
        core)

add_executable(IteratorTest IteratorTest.cpp)
target_link_libraries(IteratorTest PRIVATE
        CONAN_PKG::catch2
        core)

add_executable(RepositoryTest RepositoryTest.cpp)
target_link_libraries(RepositoryTest PRIVATE
        CONAN_PKG::catch2
        core)

add_executable(ProviderTest ProviderTest.cpp)
target_link_libraries(ProviderTest PRIVATE
        CONAN_PKG::catch2
        core)


add_executable(ResolverTest ResolverTest.cpp)
target_link_libraries(ResolverTest PRIVATE
        CONAN_PKG::catch2
        core)

add_executable(CurlProviderTest CurlProviderTest.cpp)
target_link_libraries(CurlProviderTest PRIVATE
        CONAN_PKG::catch2
        curl
        core)

add_executable(CurlResolverTest CurlResolverTest.cpp)
target_link_libraries(CurlResolverTest PRIVATE
        CONAN_PKG::catch2
        curl
        core)

add_custom_target(check
        COMMAND SignatureTest
        COMMAND GeneratorTest
        COMMAND IteratorTest
        COMMAND ProviderTest
        COMMAND RepositoryTest
        COMMAND ResolverTest
        COMMAND CurlProviderTest
        COMMAND CurlResolverTest
        )