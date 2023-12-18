add_library(ember-external-mojoal
        mojoal.c)
target_include_directories(ember-external-mojoal PUBLIC AL)
target_link_libraries(ember-external-mojoal PUBLIC
        SDL2::SDL2)


