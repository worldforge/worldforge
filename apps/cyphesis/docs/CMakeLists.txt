add_subdirectory(man)

#Add a custom target which regenerates the Python stubs in the "dox/python" directory based on the annotations we put in our C++ bindings.
#This target isn't run automatically because we don't want anything that alters the source code to do so.
#That means that you need to don't forget to run this yourself whenever you update the bindings.
add_custom_target(GeneratePythonDocs
        COMMENT "Generating Python docs stubs."
        COMMAND cyphesis-pythondocgenerator atlas > atlas.py
        COMMAND cyphesis-pythondocgenerator server > server.py
        COMMAND cyphesis-pythondocgenerator entity_filter > entity_filter.py
        COMMAND cyphesis-pythondocgenerator common > common.py
        COMMAND cyphesis-pythondocgenerator ai > ai.py
        COMMAND cyphesis-pythondocgenerator physics > physics.py
        COMMAND cyphesis-pythondocgenerator rules > rules.py
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/apps/cyphesis/docs/python
        DEPENDS cyphesis-pythondocgenerator cyphesis-rulesai_python cyphesis-rulessimulation_python cyphesis-entityfilter_python cyphesis-rulesbase_python
        VERBATIM
        )

