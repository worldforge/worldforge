add_library(ember-domain
        EmberEntity.cpp
        EntityTalk.cpp
        EmberEntityRef.cpp
)

target_link_libraries(ember-domain PUBLIC
        eris
        AtlasObjects
)

add_library(ember-BindingsDomain OBJECT
        bindings/lua/BindingsDomain.cpp
)

target_link_libraries(ember-BindingsDomain PUBLIC
        ember-domain
)
if (MSVC)
    #Need to allow for a lot of symbols when compiling the bindings on MSVC.
    target_compile_options(ember-BindingsDomain PRIVATE "/bigobj")
endif ()
