add_library(ember-BindingsVarconf OBJECT
        BindingsVarconf.cpp)

target_precompile_headers(ember-BindingsVarconf
        REUSE_FROM
        ember-BindingsLua)

target_link_libraries(ember-BindingsVarconf PUBLIC
        varconf)

if (MSVC)
    #Need to allow for a lot of symbols when compiling the bindings on MSVC.
    target_compile_options(ember-BindingsVarconf PRIVATE "/bigobj")
endif ()