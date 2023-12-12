add_library(ember-framework OBJECT
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
add_subdirectory(bindings)

target_link_libraries(ember-framework PUBLIC
        external-tinyxml
        external-utf8
        external-dirmonitor
        AtlasObjects
        eris
        varconf
        spdlog::spdlog
)

if (NOT WIN32)
    target_link_libraries(ember-framework PUBLIC
            libxdg-basedir::libxdg-basedir
    )
endif ()


#wf_generate_lua_bindings(bindings/lua/wfmath/WFMath)


