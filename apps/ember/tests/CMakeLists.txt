if (TARGET cppunit::cppunit)

    MESSAGE(STATUS "Building tests.")

    add_executable(TestOgreView
            TestOgreView.cpp
            ConvertTestCase.cpp
            ModelMountTestCase.cpp
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
            $<TARGET_OBJECTS:binreloc>
            $<TARGET_OBJECTS:CEGUIOgreRenderer>
            $<TARGET_OBJECTS:CEGUITinyXMLParser>
            $<TARGET_OBJECTS:ember-DetourTileCache>
            $<TARGET_OBJECTS:ember-Detour>
            $<TARGET_OBJECTS:ember-Recast>)
    target_compile_definitions(TestOgreView PUBLIC -DLOG_TASKS)
    target_link_libraries(TestOgreView cppunit::cppunit
            Bullet::Bullet
            mojoal)
    add_test(NAME TestOgreView COMMAND TestOgreView)
    add_dependencies(check TestOgreView)

    add_executable(TestTasks
            TestTasks.cpp
            $<TARGET_OBJECTS:ember-framework>
            $<TARGET_OBJECTS:ember-tinyxml>)
    target_compile_definitions(TestTasks PUBLIC -DLOG_TASKS)
    target_link_libraries(TestTasks
            cppunit::cppunit
            mojoal
            eris)
    add_test(NAME TestTasks COMMAND TestTasks)
    add_dependencies(check TestTasks)

    add_executable(TestTimeFrame
            TestTimeFrame.cpp
            $<TARGET_OBJECTS:ember-framework>
            $<TARGET_OBJECTS:ember-tinyxml>)
    target_compile_definitions(TestTimeFrame PUBLIC -DLOG_TASKS)
    target_link_libraries(TestTimeFrame
            cppunit::cppunit
            mojoal)
    add_test(NAME TestTimeFrame COMMAND TestTimeFrame)
    add_dependencies(check TestTimeFrame)

    add_executable(TestFramework
            TestFramework.cpp
            $<TARGET_OBJECTS:ember-framework>
            $<TARGET_OBJECTS:ember-tinyxml>)
    target_compile_definitions(TestFramework PUBLIC -DLOG_TASKS -DSRCDIR="${CMAKE_CURRENT_SOURCE_DIR}")
    target_link_libraries(TestFramework
            cppunit::cppunit
            mojoal
            eris)
    add_test(NAME TestFramework COMMAND TestFramework)
    add_dependencies(check TestFramework)

    #    add_executable(TestTerrain TestTerrain.cpp)
    #    target_compile_definitions(TestTerrain PUBLIC -DLOG_TASKS)
    #    target_link_libraries(TestTerrain ${CPPUNIT_LIBRARIES} ${WF_LIBRARIES} emberogre terrain caelum pagedgeometry entitymapping lua services framework)
    #    target_include_directories(TestTerrain PUBLIC ${CPPUNIT_INCLUDE_DIRS})
    #    add_test(NAME TestTerrain COMMAND TestTerrain)

endif ()