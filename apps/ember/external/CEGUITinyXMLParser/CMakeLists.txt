add_library(ember-CEGUITinyXMLParser OBJECT
        XMLParser.cpp
        XMLParserModule.cpp
)

target_link_libraries(ember-CEGUITinyXMLParser PUBLIC
        ember-tinyxml)

