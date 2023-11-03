set(SOURCE_FILES
        varconf/config.cpp
        varconf/parse_error.cpp
        varconf/varconf.h
        varconf/variable.cpp)

set(HEADER_FILES
        varconf/config.h
        varconf/parse_error.h
        varconf/varconf.h
        varconf/varconf_defs.h
        varconf/variable.h)

wf_add_library("${PROJECT_NAME}" SOURCE_FILES HEADER_FILES)

target_link_libraries(${PROJECT_NAME} PUBLIC
        libsigcpp::sigc++)

