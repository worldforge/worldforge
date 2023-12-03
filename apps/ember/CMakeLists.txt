project(Ember)

# Meta data
set(DESCRIPTION "The Worldforge client.")

include(GNUInstallDirs)
include(CMakeDependentOption)

add_compile_definitions("PREFIX=\"${CMAKE_INSTALL_PREFIX}\"")
add_compile_definitions("EMBER_DATADIR=\"${CMAKE_INSTALL_FULL_DATADIR}\"")
add_compile_definitions("EMBER_LIBDIR=\"${CMAKE_INSTALL_FULL_LIBDIR}\"")
add_compile_definitions("EMBER_SYSCONFDIR=\"${CMAKE_INSTALL_FULL_SYSCONFDIR}\"")

include_directories(${PROJECT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/generated)

#On Unix we'll automatically build all widgets as reloadable modules, but not on Windows or OSX (since we couldn't get it to work as we would like).
cmake_dependent_option(WF_USE_WIDGET_PLUGINS "Build widgets as reloadable plugins." ON "UNIX AND NOT APPLE;NOT CONAN_FOUND" OFF)

#Needs to declare a GLOBAL variable to pass plugin list down to the src/main/CMakeFile.
define_property(GLOBAL PROPERTY PLUGIN_LIBS_LIST
        BRIEF_DOCS "Plugins registered."
        FULL_DOCS "Plugins registered.")
set_property(GLOBAL PROPERTY PLUGIN_LIBS_LIST "")


macro(wf_add_plugin _LIB_NAME _SOURCE_FILES)

    add_library(${_LIB_NAME} MODULE ${_SOURCE_FILES})
    target_compile_options(${_LIB_NAME} PRIVATE ${WF_WARNING_FLAGS})

    install(TARGETS ${_LIB_NAME}
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/ember/widgets
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/ember/widgets)

    if (WIN32)
        #We need to tell msvc to ignore any problems with resolving symbols since they will be resolved when the plugin is loaded.
        #TODO: make the symbols exported anyway so this isn't needed
        set_target_properties(${_LIB_NAME} PROPERTIES LINK_FLAGS "/FORCE:UNRESOLVED")
    elseif ()
        set_target_properties(${_LIB_NAME} PROPERTIES LINK_FLAGS "-Wl,-undefined -Wl,dynamic_lookup \
    -Wl,-no_pie \
    -Wl,-search_paths_first")
    endif ()

    set_property(GLOBAL APPEND PROPERTY PLUGIN_LIBS_LIST "${_LIB_NAME}")
    message(STATUS "Registered widget plugin ${_LIB_NAME}")

    #Remove all linking to libraries
    set_target_properties(${_LIB_NAME} PROPERTIES LINK_LIBRARIES "")
    #And copy the include directories from the emberogre target.
    target_include_directories(${_LIB_NAME} PRIVATE $<TARGET_PROPERTY:emberogre,INTERFACE_INCLUDE_DIRECTORIES>)

endmacro()

if (WIN32)
    link_libraries(Shlwapi) #Needed for getting Windows specific directories.
    add_compile_definitions("_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING") #CEGUI has a String iterator that inherits from std::iterator, which shouldn't be done. We don't want to see the warning from MSVC all the time.
endif ()

add_compile_definitions(AL_LIBTYPE_STATIC)

find_package(Boost
        1.83.0
        REQUIRED
        COMPONENTS system filesystem thread)

link_libraries(Boost::boost)
link_libraries(Boost::filesystem)

#Check for libunwind, which is optional and if present will allow for the StackChecker feature to be enabled.
#This allows for a developer to get some insight into why some frames take too long.
find_package(unwind 1.1)
if (UNWIND_FOUND)
    message(STATUS "Found libunwind which allows for the 'slow frame stack print' feature to be enabled.")
    link_directories(${UNWIND_LIBRARY_DIRS})
    link_libraries(${UNWIND_LIBRARIES})
    include_directories(SYSTEM ${UNWIND_INCLUDE_DIR})
    add_compile_definitions("UNWIND_ENABLED")
endif (UNWIND_FOUND)


find_package(SDL2 REQUIRED)
include_directories(SYSTEM ${SDL2_INCLUDE_DIR})
link_directories(${SDL2_LIBRARY_DIRS})
link_libraries(${SDL2_LIBRARIES})

find_package(sigc++-3 3.0 REQUIRED)
link_libraries(libsigcpp::sigc++)


find_package(OGRE
        13.4.2
        REQUIRED)
if (CONAN_FOUND)
    #With Conan there will be a hard coded path to Freetype within the Overlay target. Let's remove that and link to
    # Freetype ourselves.
    find_package(Freetype CONFIG REQUIRED)
    set_target_properties(OgreOverlay PROPERTIES
            INTERFACE_LINK_LIBRARIES "OgreMain;$<LINK_ONLY:Freetype::Freetype>"
    )

    #If we're building with Conan we're statically linking Ogre, and need to link to the RenderSystem.
    link_libraries(RenderSystem_GL3Plus)
endif ()

link_libraries(OgreMain)

if (APPLE)
    if (NOT OGRE_RenderSystem_GL3Plus_FOUND)
        MESSAGE(FATAL_ERROR "Could not find Ogre RenderSystem GL3Plus plugin. Make sure you've built Ogre with RenderSystem GL3Plus support.")
    endif (NOT OGRE_RenderSystem_GL3Plus_FOUND)
endif (APPLE)

if (NOT OGRE_Terrain_FOUND)
    MESSAGE(FATAL_ERROR "Could not find Ogre Terrain component. Make sure you've built Ogre with Terrain support.")
endif (NOT OGRE_Terrain_FOUND)
link_libraries(OgreTerrain)

if (NOT OGRE_Overlay_FOUND)
    MESSAGE(FATAL_ERROR "Could not find Ogre Overlay component. Make sure you've built Ogre with Overlay support.")
endif (NOT OGRE_Overlay_FOUND)
link_libraries(OgreOverlay)

if (NOT OGRE_MeshLodGenerator_FOUND)
    MESSAGE(FATAL_ERROR "Could not find Ogre MeshLodGenerator component. Make sure you've built Ogre with MeshLodGenerator support.")
endif (NOT OGRE_MeshLodGenerator_FOUND)
link_libraries(OgreMeshLodGenerator)

if (NOT OGRE_RTShaderSystem_FOUND)
    MESSAGE(FATAL_ERROR "Could not find Ogre RTShaderSystem component. Make sure you've built Ogre with RTShaderSystem support.")
endif (NOT OGRE_RTShaderSystem_FOUND)
link_libraries(OgreRTShaderSystem)

if (NOT OGRE_Codec_STBI_FOUND)
    MESSAGE(FATAL_ERROR "Could not find Ogre STBI Codec. Make sure you've built Ogre with STBI support.")
endif (NOT OGRE_Codec_STBI_FOUND)
link_libraries(Codec_STBI)

if (NOT OGRE_Plugin_ParticleFX_FOUND)
    MESSAGE(FATAL_ERROR "Could not find Ogre ParticleFX Plugin. Make sure you've built Ogre with ParticleFX support.")
endif (NOT OGRE_Plugin_ParticleFX_FOUND)
link_libraries(Plugin_ParticleFX)

#I just couldn't get Windows to find Lua when provided by Conan, no matter what I tried. As a fallback we'll instead
# use the auto-generated "findlua.cmake" provided by the Conan "cmake_find_package" generator. Preferably this should
# instead be handled by CMake and the paths provided by the "cmake_paths" generator. But here we are.
#Seems also be true for MacOS, so we'll enable this for all platforms if Conan is used.
if (CONAN_FOUND)
    message(STATUS "Looking for Lua through Conan.")
    find_package(lua 5.3.6 EXACT REQUIRED)
    link_libraries(lua::lua)
else ()
    find_package(Lua 5.3 REQUIRED)
    link_directories(${LUA_LIBRARY_DIRS})
    link_libraries(${LUA_LIBRARIES})
    include_directories(SYSTEM ${LUA_INCLUDE_DIR})
endif ()
#If we're using Conan we should use the targets as provided by Conan. Otherwise we'll use the legacy FindCEGUI mechanism.
if (CONAN_FOUND)
    find_package(CEGUI
            0.8.7
            CONFIG
            REQUIRED)
    link_libraries(cegui::cegui)
else ()
    find_package(CEGUI
            0.8.7
            REQUIRED)
    link_directories(${CEGUI_LIBRARY_DIRS})
    link_libraries(${CEGUI_LIBRARIES})
    include_directories(SYSTEM ${CEGUI_INCLUDE_DIRS})
endif ()

if (NOT WIN32)
    find_package(libxdg-basedir REQUIRED)
endif ()

find_package(OpenGL)
link_libraries(OpenGL::GL)

find_package(spdlog REQUIRED)

find_package(Bullet 2.89 REQUIRED)
include_directories(SYSTEM ${BULLET_INCLUDE_DIRS})
link_libraries(${BULLET_LIBRARIES})


if (WF_USE_WIDGET_PLUGINS)
    message(STATUS "Widget Plugins will be built dynamically")
    # This is needed to make Widget Plugins work.
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    add_compile_definitions("WF_USE_WIDGET_PLUGINS")
endif ()

#We'll use xmllint for validating schemas of some of our xml files.
find_program(XMLLINT xmllint)


set(LOCAL_OGRE_MEDIA_DIR ${CMAKE_INSTALL_FULL_DATADIR}/ember/OGRE/Media)
message(STATUS "Copying OGRE media (from ${OGRE_MEDIA_DIR}) to our own installation directory (${LOCAL_OGRE_MEDIA_DIR}).")
install(DIRECTORY "${OGRE_MEDIA_DIR}/RTShaderLib" DESTINATION "${LOCAL_OGRE_MEDIA_DIR}")
install(DIRECTORY "${OGRE_MEDIA_DIR}/Main" DESTINATION "${LOCAL_OGRE_MEDIA_DIR}")

add_compile_definitions("OGRE_PLUGINDIR=\"${OGRE_PLUGIN_DIR}\"")

#It would be better if Bullet provided a header which specified how it was built...
OPTION(BT_USE_DOUBLE_PRECISION "Using double precision with Bullet. This should match what Bullet has been compiled with." OFF)

if (BT_USE_DOUBLE_PRECISION)
    message(STATUS "Using double precision with Bullet. Make sure that Bullet is compiled with this too.")
    add_compile_definitions("BT_USE_DOUBLE_PRECISION=1")
endif ()


add_subdirectory(external)
add_subdirectory(src)
#TODO: enable again once we've sorted the monorepo
#add_subdirectory(tests)

file(GLOB MODELDEFINITIONS_FILES data/dural/*.modeldef)
foreach (FILE ${MODELDEFINITIONS_FILES})
    add_custom_command(TARGET check
            COMMAND "${XMLLINT}" --schema "${PROJECT_SOURCE_DIR}/data/modeldefinition.xsd" --nonet --noout "${FILE}")
endforeach ()

configure_file(tools/Version.tmpl.h generated/Version.h @ONLY)
configure_file(tools/bintray.tmpl.json bintray.json @ONLY)
message(STATUS "Installing snapcraft.yaml file into build directory. Copy this one to the installation directory if you want to build snaps.")
configure_file(tools/snapcraft.tmpl.yaml snap/snapcraft.yaml @ONLY)
configure_file(tools/ember.tmpl.conf ember.conf @ONLY)
configure_file(tools/Ember.tmpl.nsi Ember.nsi @ONLY)
configure_file(docs/Doxyfile.in Doxyfile @ONLY)
configure_file(tools/support/refresh-amber-media-dev.sh.in scripts/refresh-amber-media-dev.sh @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ember.conf DESTINATION ${CMAKE_INSTALL_FULL_SYSCONFDIR}/ember)
install(FILES tools/org.worldforge.ember.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
install(FILES data/media/ember.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/64x64/apps)
install(FILES README.md COPYING AUTHORS DESTINATION ${CMAKE_INSTALL_DATADIR}/doc/ember)
install(FILES tools/org.worldforge.ember.appdata.xml DESTINATION ${CMAKE_INSTALL_DATADIR}/metainfo)

install(DIRECTORY data DESTINATION ${CMAKE_INSTALL_FULL_DATADIR}/ember)

file(GLOB SOUND_DEFINITION_FILES "${CMAKE_CURRENT_SOURCE_DIR}/sounddefinitions/*.sounddef")
install(FILES ${SOUND_DEFINITION_FILES} DESTINATION ${CMAKE_INSTALL_FULL_DATADIR}/ember/sounddefinitions)

# man files
install(FILES docs/man/man1/ember.1 DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)

find_package(Subversion)
find_package(Python3 COMPONENTS Interpreter)
find_program(RSYNC_CMD NAMES rsync)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    #On systems with Snapcraft this command will build a snap, otherwise one has to do some extra copying of the "snap" folder.
    add_custom_target(snap-build-ember)
    add_custom_command(
            TARGET snap-build-ember
            COMMAND ${CMAKE_COMMAND} -E echo "I will now copy the snapfile to the installation directory and run 'snapcraft'."
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_INSTALL_PREFIX}/snap
            COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/snap/snapcraft.yaml ${CMAKE_INSTALL_PREFIX}/snap/snapcraft.yaml
            COMMAND ${SNAPCRAFT_COMMAND}
            WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}
    )
endif ()

# Doxygen support, exports a "dox" target.

find_package(Doxygen)

if (DOXYGEN_FOUND)

    set(DOXYGEN_INPUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    set(DOXYGEN_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/docs)

    add_custom_command(
            OUTPUT ${DOXYGEN_OUTPUT}
            COMMAND ${CMAKE_COMMAND} -E echo_append "Building API Documentation..."
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_INPUT}
            COMMAND ${CMAKE_COMMAND} -E echo "Done."
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            DEPENDS ${DOXYGEN_INPUT}
    )

#    add_custom_target(dox DEPENDS ${DOXYGEN_OUTPUT})

endif (DOXYGEN_FOUND)

add_custom_command(
        OUTPUT ChangeLog
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/generate-ChangeLog.sh ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} f12012e7616c191a8926432faf866c8e43854062
)
#add_custom_target(changelog DEPENDS ChangeLog)

