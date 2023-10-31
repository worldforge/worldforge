add_library(ember-BindingsLua OBJECT
        BindingsLua.cpp)

target_precompile_headers(ember-BindingsLua
        PUBLIC
        "${PROJECT_SOURCE_DIR}/external/sol2/sol.hpp")

if (MSVC)
    #Need to allow for a lot of symbols when compiling the bindings on MSVC.
    target_compile_options(ember-BindingsLua PRIVATE "/bigobj")
endif ()