add_definitions(-DTESTDATADIR="${CMAKE_CURRENT_SOURCE_DIR}/data")

add_executable(GeneratorTest GeneratorTest.cpp)
target_link_libraries(GeneratorTest PRIVATE
        CONAN_PKG::catch2
        core)