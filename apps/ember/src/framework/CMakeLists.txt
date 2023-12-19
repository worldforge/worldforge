add_library(ember-framework STATIC
        AttributeObserver.cpp
        ConsoleBackend.cpp
        ConsoleCommandWrapper.cpp
        DeepAttributeObserver.cpp
        DirectAttributeObserver.cpp
        Tokeniser.cpp
        TimedLog.cpp
        TimeHelper.cpp
        TimeFrame.cpp
        CommandHistory.cpp
        MainLoopController.cpp
        FileResourceProvider.cpp
        EntityExporterBase.cpp
        EntityExporter.cpp
        EntityImporterBase.cpp
        EntityImporter.cpp
        AtlasMessageLoader.cpp
        TinyXmlCodec.cpp
        AtlasObjectDecoder.cpp
        tasks/TaskExecutor.cpp
        tasks/TaskExecutionContext.cpp
        tasks/TaskQueue.cpp
        tasks/TaskUnit.cpp
        tasks/SerialTask.cpp
        FileSystemObserver.cpp
        StackChecker.cpp
        LogExtensions.cpp
        Log.cpp
)

#Check for libunwind, which is optional and if present will allow for the StackChecker feature to be enabled.
#This allows for a developer to get some insight into why some frames take too long.
find_package(unwind 1.1)
if (UNWIND_FOUND)
    message(STATUS "Found libunwind which allows for the 'slow frame stack print' feature to be enabled.")
    target_link_libraries(ember-framework PUBLIC unwind)
    target_compile_definitions(ember-framework PRIVATE UNWIND_ENABLED)
endif (UNWIND_FOUND)

add_subdirectory(bindings)

target_link_libraries(ember-framework PUBLIC
        external-tinyxml
        external-utf8
        external-dirmonitor
        AtlasObjects
        eris
        varconf
        spdlog::spdlog
        libsigcpp::sigc++
        Boost::filesystem
)

if (NOT WIN32)
    target_link_libraries(ember-framework PUBLIC
            libxdg-basedir::libxdg-basedir
    )
endif ()


#wf_generate_lua_bindings(bindings/lua/wfmath/WFMath)


