add_subdirectory(bindings)

set(SCRIPT_LIBRARIES
        $<TARGET_OBJECTS:ember-BindingsDomain>
        $<TARGET_OBJECTS:ember-BindingsServices>
        $<TARGET_OBJECTS:ember-BindingsCEGUI>
        $<TARGET_OBJECTS:ember-BindingsAtlasAdapters>
        $<TARGET_OBJECTS:ember-BindingsRepresentations>
        $<TARGET_OBJECTS:ember-BindingsFramework>
        $<TARGET_OBJECTS:ember-BindingsEris>
        $<TARGET_OBJECTS:ember-BindingsVarconf>
        $<TARGET_OBJECTS:ember-BindingsAtlas>
        $<TARGET_OBJECTS:ember-BindingsLua>
        $<TARGET_OBJECTS:ember-BindingsEmberOgre>
        $<TARGET_OBJECTS:ember-BindingsOgre>
        $<TARGET_OBJECTS:ember-BindingsWFMath>
)

add_library(ember-lua_component STATIC
        LuaScriptingProvider.cpp
        Connector.cpp
        LuaConsoleObject.cpp
        ${SCRIPT_LIBRARIES}
)

target_link_libraries(ember-lua_component PUBLIC
        eris
        ember-services
        ember-external-sol2
)

