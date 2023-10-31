if (WIN32)
    set(EMBER_EXECUTABLE_NAME ember)
else ()
    set(EMBER_EXECUTABLE_NAME ember.bin)
endif ()


#Use WIN32 to specify that on Windows it should be a non-console app.
add_executable(${EMBER_EXECUTABLE_NAME} WIN32)
target_sources(${EMBER_EXECUTABLE_NAME} PRIVATE
        Application.cpp
        ConfigBoundLogObserver.cpp
        Ember.cpp
        ConsoleInputBinder.cpp
        $<TARGET_OBJECTS:ember-ogre>
        $<TARGET_OBJECTS:ember-caelum>
        $<TARGET_OBJECTS:ember-meshtree>
        $<TARGET_OBJECTS:ember-pagedgeometry>
        $<TARGET_OBJECTS:ember-widgets>
        $<TARGET_OBJECTS:ember-ogre_terrain>
        $<TARGET_OBJECTS:ember-entitymapping>
        $<TARGET_OBJECTS:ember-cegui>
        $<TARGET_OBJECTS:ember-navigation>
        $<TARGET_OBJECTS:ember-services>
        $<TARGET_OBJECTS:ember-domain>
        $<TARGET_OBJECTS:ember-framework>
        $<TARGET_OBJECTS:ember-tinyxml>
        $<TARGET_OBJECTS:ember-binreloc>
        $<TARGET_OBJECTS:ember-CEGUIOgreRenderer>
        $<TARGET_OBJECTS:ember-DetourTileCache>
        $<TARGET_OBJECTS:ember-Detour>
        $<TARGET_OBJECTS:ember-Recast>)

target_link_libraries(${EMBER_EXECUTABLE_NAME}
        ember-assets
        ember-lua_component
        mercator
        mojoal
        ember-CEGUITinyXMLParser
        ${BULLET_LIBRARIES})

install(TARGETS ${EMBER_EXECUTABLE_NAME} DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})

if (WF_USE_WIDGET_PLUGINS)

    #The idea here is to allow symbol exports from the main exe, so that the plugins can import them. Not sure if it works though.
    set_property(TARGET ${EMBER_EXECUTABLE_NAME} PROPERTY ENABLE_EXPORTS 1)

    #Need to include CMAKE_DL_LIBS to get the dynamic linker.
    target_link_libraries(${EMBER_EXECUTABLE_NAME}
            ${WF_LIBRARIES}
            ${CMAKE_DL_LIBS})

    wf_add_plugin(ServerWidget
            ${CMAKE_SOURCE_DIR}/src/components/ogre/widgets/ServerWidget.cpp
    )
    wf_add_plugin(Quit
            ${CMAKE_SOURCE_DIR}/src/components/ogre/widgets/Quit.cpp
    )
    wf_add_plugin(InspectWidget
            ${CMAKE_SOURCE_DIR}/src/components/ogre/widgets/InspectWidget.cpp
    )

    wf_add_plugin(Help
            ${CMAKE_SOURCE_DIR}/src/components/ogre/widgets/Help.cpp
    )

    wf_add_plugin(ContainerWidget
            ${CMAKE_SOURCE_DIR}/src/components/ogre/widgets/ContainerWidget.cpp
    )

    wf_add_plugin(IngameChatWidget
            ${CMAKE_SOURCE_DIR}/src/components/ogre/widgets/IngameChatWidget.cpp
    )

    wf_add_plugin(EntityCreatorWidget
            ${CMAKE_SOURCE_DIR}/src/components/ogre/widgets/EntityCreatorWidget.cpp
    )
    get_property(_PLUGIN_LIBS_LIST GLOBAL PROPERTY PLUGIN_LIBS_LIST)

    #add_dependencies(${EMBER_EXECUTABLE_NAME} ${_PLUGIN_LIBS_LIST})
    foreach (PLUGIN ${_PLUGIN_LIBS_LIST})
        add_dependencies(${PLUGIN} ${EMBER_EXECUTABLE_NAME})
    endforeach (PLUGIN)

endif ()