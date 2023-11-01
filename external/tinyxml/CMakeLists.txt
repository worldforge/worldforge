add_library(external-tinyxml OBJECT
        tinyxml/tinystr.cpp
        tinyxml/tinyxml.cpp
        tinyxml/tinyxmlerror.cpp
        tinyxml/tinyxmlparser.cpp
)

target_include_directories(external-tinyxml PUBLIC .)
