add_library(ember-lua-console STATIC
        LuaConsoleObject.cpp
)
target_link_libraries(ember-lua-console PUBLIC
        ember-framework
        ember-external-sol2
)

add_subdirectory(bindings)


add_library(ember-lua_component STATIC
        LuaScriptingProvider.cpp
)


target_link_libraries(ember-lua_component PUBLIC
        ember-BindingsDomain
        ember-BindingsServices
        ember-BindingsCEGUI
        ember-BindingsAtlasAdapters
        ember-BindingsRepresentations
        ember-BindingsFramework
        ember-BindingsEris
        ember-BindingsVarconf
        ember-BindingsAtlas
        ember-BindingsLua
        ember-BindingsEmberOgre
        ember-BindingsOgre
        ember-BindingsWFMath
        ember-lua-console
        ember-framework
        ember-services
        ember-external-sol2
        eris
)

