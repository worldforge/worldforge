add_library(ember-BindingsAtlas OBJECT
        BindingsAtlas.cpp
)

target_link_libraries(ember-BindingsAtlas PUBLIC
        ember-external-sol2
        AtlasObjects
)

target_precompile_headers(ember-BindingsAtlas
        REUSE_FROM
        ember-BindingsLua
)

if (MSVC)
    #Need to allow for a lot of symbols when compiling the bindings on MSVC.
    target_compile_options(ember-BindingsAtlas PRIVATE "/bigobj")
endif ()