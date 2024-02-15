find_package(Boost
        1.84.0
        REQUIRED
        COMPONENTS thread)

add_library(external-dirmonitor INTERFACE)

target_include_directories(external-dirmonitor INTERFACE include)

target_link_libraries(external-dirmonitor INTERFACE
        Boost::thread
        Boost::headers
)
