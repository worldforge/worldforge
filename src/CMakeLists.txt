include_directories("${PROJECT_SOURCE_DIR}/src")
# Define source files for libraries

set(CODECS_SOURCE_FILES
        Atlas/Codecs/Bach.cpp
        Atlas/Codecs/Packed.cpp
        Atlas/Codecs/XML.cpp)

set(CODECS_HEADER_FILES
        Atlas/Codecs/Bach.h
        Atlas/Codecs/Packed.h
        Atlas/Codecs/Utility.h
        Atlas/Codecs/XML.h)

set(OBJECTS_SOURCE_FILES
        Atlas/Objects/Anonymous.cpp
        Atlas/Objects/BaseObject.cpp
        Atlas/Objects/Decoder.cpp
        Atlas/Objects/Generic.cpp
        Atlas/Objects/loadDefaults.cpp
        Atlas/Objects/Factories_generated.cpp
        Atlas/Objects/Factories.cpp
        Atlas/Objects/Root.cpp
        Atlas/Objects/RootEntity.cpp
        Atlas/Objects/RootEntityChildren.cpp
        Atlas/Objects/RootOperation.cpp
        Atlas/Objects/RootOperationChildren1.cpp
        Atlas/Objects/RootOperationChildren2.cpp
        Atlas/Objects/RootOperationChildren3.cpp
        Atlas/Objects/RootOperationChildren4.cpp
        Atlas/Objects/RootOperationChildren5.cpp
        Atlas/Objects/RootOperationChildren6.cpp
        )

set(OBJECTS_HEADER_FILES
        Atlas/Objects/Anonymous.h
        Atlas/Objects/BaseObject.h
        Atlas/Objects/Decoder.h
        Atlas/Objects/Encoder.h
        Atlas/Objects/Entity.h
        Atlas/Objects/Generic.h
        Atlas/Objects/loadDefaults.h
        Atlas/Objects/Factories.h
        Atlas/Objects/ObjectsFwd.h
        Atlas/Objects/Operation.h
        Atlas/Objects/Root.h
        Atlas/Objects/RootEntity.h
        Atlas/Objects/RootOperation.h
        Atlas/Objects/SmartPtr.h)

set(FILTERS_SOURCE_FILES
        Atlas/Filters/Bzip2.cpp
        Atlas/Filters/Gzip.cpp)

set(FILTERS_HEADER_FILES
        Atlas/Filters/Bzip2.h
        Atlas/Filters/Gzip.h)

set(FUNKY_SOURCE_FILES
        Atlas/Funky/FEncoder.cpp)

set(FUNKY_HEADER_FILES
        Atlas/Funky/FEncoder.h)

set(MESSAGE_SOURCE_FILES
        Atlas/Message/DecoderBase.cpp
        Atlas/Message/Element.cpp
        Atlas/Message/MEncoder.cpp
        Atlas/Message/QueuedDecoder.cpp)

set(MESSAGE_HEADER_FILES
        Atlas/Message/DecoderBase.h
        Atlas/Message/Element.h
        Atlas/Message/MEncoder.h
        Atlas/Message/QueuedDecoder.h)

set(NET_SOURCE_FILES
        Atlas/Net/Loopback.cpp
        Atlas/Net/Stream.cpp)

set(NET_HEADER_FILES
        Atlas/Net/Loopback.h
        Atlas/Net/Stream.h)

set(SOURCE_FILES
        Atlas/Codec.h
        Atlas/EncoderBase.h
        Atlas/Filter.cpp
        Atlas/float.cpp
        Atlas/Formatter.cpp
        Atlas/MultiLineListFormatter.cpp
        Atlas/PresentationBridge.cpp)

set(HEADER_FILES
        Atlas/Bridge.h
        Atlas/Codec.h
        Atlas/Debug.h
        Atlas/EncoderBase.h
        Atlas/Exception.h
        Atlas/Filter.h
        Atlas/float.h
        Atlas/Formatter.h
        Atlas/MultiLineListFormatter.h
        Atlas/Negotiate.h
        Atlas/PresentationBridge.h
        Atlas/Version.h)


wf_add_library(Atlas${SUFFIX} SOURCE_FILES HEADER_FILES)

wf_add_library(AtlasCodecs${SUFFIX} CODECS_SOURCE_FILES CODECS_HEADER_FILES)
target_link_libraries(AtlasCodecs${SUFFIX} Atlas${SUFFIX})

wf_add_library(AtlasNet${SUFFIX} NET_SOURCE_FILES NET_HEADER_FILES)
target_link_libraries(AtlasNet${SUFFIX} Atlas${SUFFIX} AtlasCodecs${SUFFIX})

wf_add_library(AtlasMessage${SUFFIX} MESSAGE_SOURCE_FILES MESSAGE_HEADER_FILES)
target_link_libraries(AtlasMessage${SUFFIX} Atlas${SUFFIX})

wf_add_library(AtlasObjects${SUFFIX} OBJECTS_SOURCE_FILES OBJECTS_HEADER_FILES)
target_link_libraries(AtlasObjects${SUFFIX} AtlasMessage${SUFFIX} AtlasCodecs${SUFFIX})
if (PYTHON_EXECUTABLE)
    add_dependencies(AtlasObjects${SUFFIX} GenerateAtlasXml GenerateAtlasObjects)
    add_dependencies(GenerateAtlasObjects GenerateAtlasXml)
endif (PYTHON_EXECUTABLE)

wf_add_library(AtlasFilters${SUFFIX} FILTERS_SOURCE_FILES FILTERS_HEADER_FILES)
target_link_libraries(AtlasFilters${SUFFIX} Atlas${SUFFIX})

if (BZIP2_FOUND)
    include_directories(${BZIP2_INCLUDE_DIRS})
    target_link_libraries(AtlasFilters${SUFFIX} ${BZIP2_LIBRARIES})
    add_definitions(-DHAVE_BZLIB_H -DHAVE_LIBBZ2)
endif (BZIP2_FOUND)

if (ZLIB_FOUND)
    include_directories(${ZLIB_INCLUDE_DIRS})
    target_link_libraries(AtlasFilters${SUFFIX} ${ZLIB_LIBRARIES})
    add_definitions(-DHAVE_ZLIB_H -DHAVE_LIBZ)
endif (ZLIB_FOUND)

# The "atlas_convert" tool
add_executable(atlas_convert atlas_convert.cpp)
target_compile_options(atlas_convert PRIVATE ${WF_WARNING_FLAGS})
target_link_libraries(atlas_convert AtlasCodecs${SUFFIX} AtlasMessage${SUFFIX} Atlas${SUFFIX})

install(TARGETS atlas_convert
        DESTINATION ${CMAKE_INSTALL_BINDIR})