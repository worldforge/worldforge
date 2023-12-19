#Use WIN32 to specify that on Windows it should be a non-console app.
add_executable(ember WIN32)
target_sources(ember PRIVATE
        Application.cpp
        ConfigBoundLogObserver.cpp
        Ember.cpp
        ConsoleInputBinder.cpp
)

target_link_libraries(ember
        ember-ogre
        ember-widgets
        ember-ogre_terrain
        ember-entitymapping
        ember-cegui
        ember-navigation
        ember-services
        ember-domain
        ember-framework
        ember-external-caelum
        ember-external-CEGUIOgreRenderer
        ember-external-meshtree
        ember-external-pagedgeometry
        external-tinyxml
        external-binreloc
        external-DetourTileCache
        external-DetourCrowd
        external-Recast        
        
        ember-assets
        ember-lua_component
        mercator
        ember-external-mojoal
        ember-external-CEGUITinyXMLParser
        external-Detour
        Boost::url
        OgreMain)

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