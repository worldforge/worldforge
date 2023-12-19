add_library(ember-BindingsLua STATIC
        BindingsLua.cpp)

target_link_libraries(ember-BindingsLua PUBLIC
        ember-lua-console
        ember-framework
        ember-external-sol2
)

target_precompile_headers(ember-BindingsLua
        PUBLIC
        <sol2/sol.hpp>
)

if (MSVC)
    #Need to allow for a lot of symbols when compiling the bindings on MSVC.
    target_compile_options(ember-BindingsLua PRIVATE "/bigobj")
endif ()