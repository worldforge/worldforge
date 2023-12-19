add_library(ember-BindingsWFMath STATIC
        BindingsWFMath.cpp)
target_link_libraries(ember-BindingsWFMath PUBLIC
        ember-framework
        ember-external-sol2
        wfmath
)
target_precompile_headers(ember-BindingsWFMath
        REUSE_FROM
        ember-BindingsLua
)


if (MSVC)
    #Need to allow for a lot of symbols when compiling the bindings on MSVC.
    target_compile_options(ember-BindingsWFMath PRIVATE "/bigobj")
endif ()