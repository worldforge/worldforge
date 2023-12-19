add_library(ember-external-sol2 INTERFACE)

target_include_directories(ember-external-sol2 INTERFACE .)

target_link_libraries(ember-external-sol2 INTERFACE
        lua::lua)
