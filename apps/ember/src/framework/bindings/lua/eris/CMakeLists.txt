file(GLOB BINDINGS_FILES *.cpp)

add_library(ember-BindingsEris OBJECT
        ${BINDINGS_FILES})

target_link_libraries(ember-BindingsEris PUBLIC
        ember-framework
        ember-external-sol2
        eris
)

target_precompile_headers(ember-BindingsEris
        REUSE_FROM
        ember-BindingsLua
)
if (MSVC)
    #Need to allow for a lot of symbols when compiling the bindings on MSVC.
    target_compile_options(ember-BindingsEris PRIVATE "/bigobj")
endif ()