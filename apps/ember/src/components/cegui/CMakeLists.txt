add_library(ember-cegui STATIC
        CEGUILogger.cpp
        ColouredRenderedStringParser.cpp
        RenderedColourStringTextComponent.cpp
        SDLNativeClipboardProvider.cpp
        CEGUISetup.cpp
)

target_link_libraries(ember-cegui PUBLIC
        ember-services
        ember-external-CEGUITinyXMLParser
        ember-external-CEGUIOgreRenderer
        varconf
        cegui::cegui
)

file(GLOB CEGUI_FILES bindings/lua/*.cpp)

add_library(ember-BindingsCEGUI STATIC
        ${CEGUI_FILES}
)

target_link_libraries(ember-BindingsCEGUI PUBLIC
        ember-framework
        ember-external-sol2
        cegui::cegui
)

target_precompile_headers(ember-BindingsCEGUI
        PUBLIC
        <CEGUI/CEGUI.h>
        <sol2/sol.hpp>
)

if (MSVC)
    #Need to allow for a lot of symbols when compiling the bindings on MSVC.
    target_compile_options(ember-BindingsCEGUI PRIVATE "/bigobj")
endif ()

install(DIRECTORY datafiles DESTINATION ${CMAKE_INSTALL_FULL_DATADIR}/ember/gui/cegui)
install(DIRECTORY schemas DESTINATION ${CMAKE_INSTALL_FULL_DATADIR}/ember/gui)

