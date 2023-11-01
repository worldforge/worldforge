add_library(ember-external-CEGUITinyXMLParser OBJECT
        CEGUITinyXMLParser/XMLParser.cpp
        CEGUITinyXMLParser/XMLParserModule.cpp
)

target_link_libraries(ember-external-CEGUITinyXMLParser PUBLIC
        external-tinyxml)

target_include_directories(ember-external-CEGUITinyXMLParser PUBLIC .)

