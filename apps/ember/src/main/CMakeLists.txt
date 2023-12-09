#Use WIN32 to specify that on Windows it should be a non-console app.
add_executable(ember WIN32)
target_sources(ember PRIVATE
        Application.cpp
        ConfigBoundLogObserver.cpp
        Ember.cpp
        ConsoleInputBinder.cpp
        $<TARGET_OBJECTS:ember-ogre>
        $<TARGET_OBJECTS:ember-widgets>
        $<TARGET_OBJECTS:ember-ogre_terrain>
        $<TARGET_OBJECTS:ember-entitymapping>
        $<TARGET_OBJECTS:ember-cegui>
        $<TARGET_OBJECTS:ember-navigation>
        $<TARGET_OBJECTS:ember-services>
        $<TARGET_OBJECTS:ember-domain>
        $<TARGET_OBJECTS:ember-framework>
        $<TARGET_OBJECTS:ember-external-caelum>
        $<TARGET_OBJECTS:ember-external-CEGUIOgreRenderer>
        $<TARGET_OBJECTS:ember-external-meshtree>
        $<TARGET_OBJECTS:ember-external-pagedgeometry>
        $<TARGET_OBJECTS:external-tinyxml>
        $<TARGET_OBJECTS:external-binreloc>
        $<TARGET_OBJECTS:external-DetourTileCache>
        $<TARGET_OBJECTS:external-DetourCrowd>
        $<TARGET_OBJECTS:external-Recast>)

target_link_libraries(ember
        ember-assets
        ember-lua_component
        mercator
        ember-external-mojoal
        ember-external-CEGUITinyXMLParser
        external-Detour
        Boost::url
        ${BULLET_LIBRARIES})

install(TARGETS ember DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})

if (WF_USE_WIDGET_PLUGINS)

    #The idea here is to allow symbol exports from the main exe, so that the plugins can import them. Not sure if it works though.
    set_property(TARGET ember PROPERTY ENABLE_EXPORTS 1)

    #Need to include CMAKE_DL_LIBS to get the dynamic linker.
    target_link_libraries(ember
            ${CMAKE_DL_LIBS})

    wf_add_plugin(ServerWidget
            ${CMAKE_CURRENT_SOURCE_DIR}/../components/ogre/widgets/ServerWidget.cpp
    )
    wf_add_plugin(Quit
            ${CMAKE_CURRENT_SOURCE_DIR}/../components/ogre/widgets/Quit.cpp
    )
    wf_add_plugin(InspectWidget
            ${CMAKE_CURRENT_SOURCE_DIR}/../components/ogre/widgets/InspectWidget.cpp
    )

    wf_add_plugin(Help
            ${CMAKE_CURRENT_SOURCE_DIR}/../components/ogre/widgets/Help.cpp
    )

    wf_add_plugin(ContainerWidget
            ${CMAKE_CURRENT_SOURCE_DIR}/../components/ogre/widgets/ContainerWidget.cpp
    )

    wf_add_plugin(IngameChatWidget
            ${CMAKE_CURRENT_SOURCE_DIR}/../components/ogre/widgets/IngameChatWidget.cpp
    )

    wf_add_plugin(EntityCreatorWidget
            ${CMAKE_CURRENT_SOURCE_DIR}/../components/ogre/widgets/EntityCreatorWidget.cpp
    )
    get_property(_PLUGIN_LIBS_LIST GLOBAL PROPERTY PLUGIN_LIBS_LIST)

    #add_dependencies(ember ${_PLUGIN_LIBS_LIST})
    foreach (PLUGIN ${_PLUGIN_LIBS_LIST})
        add_dependencies(${PLUGIN} ember)
    endforeach (PLUGIN)

endif ()