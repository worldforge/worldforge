set(CODECS_SOURCE_FILES
        Atlas/Codecs/Bach.cpp
        Atlas/Codecs/Packed.cpp
        Atlas/Codecs/XML.cpp)

set(CODECS_HEADER_FILES
        Atlas/Codecs/Bach.h
        Atlas/Codecs/Packed.h
        Atlas/Codecs/Utility.h
        Atlas/Codecs/XML.h)

file(GLOB OPERATIONS_FILES Atlas/Objects/RootOperationChildren*.cpp)
file(GLOB ENTITY_FILES Atlas/Objects/RootEntityChildren*.cpp)

set(OBJECTS_SOURCE_FILES
        Atlas/Objects/Anonymous.cpp
        Atlas/Objects/BaseObject.cpp
        Atlas/Objects/Decoder.cpp
        Atlas/Objects/Generic.cpp
        Atlas/Objects/Factories_generated.cpp
        Atlas/Objects/Factories.cpp
        Atlas/Objects/Root.cpp
        Atlas/Objects/RootEntity.cpp
        Atlas/Objects/RootOperation.cpp
        ${OPERATIONS_FILES}
        ${ENTITY_FILES}
)

set(OBJECTS_HEADER_FILES
        Atlas/Objects/Anonymous.h
        Atlas/Objects/BaseObject.h
        Atlas/Objects/Decoder.h
        Atlas/Objects/Encoder.h
        Atlas/Objects/Entity.h
        Atlas/Objects/Generic.h
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


wf_add_library(Atlas SOURCE_FILES HEADER_FILES)

wf_add_library(AtlasCodecs CODECS_SOURCE_FILES CODECS_HEADER_FILES)
target_link_libraries(AtlasCodecs Atlas)

wf_add_library(AtlasNet NET_SOURCE_FILES NET_HEADER_FILES)
target_link_libraries(AtlasNet Atlas AtlasCodecs)

wf_add_library(AtlasMessage MESSAGE_SOURCE_FILES MESSAGE_HEADER_FILES)
target_link_libraries(AtlasMessage Atlas)

wf_add_library(AtlasObjects OBJECTS_SOURCE_FILES OBJECTS_HEADER_FILES)
target_link_libraries(AtlasObjects AtlasMessage AtlasCodecs)
if (ATLAS_GENERATE_OBJECTS)
    add_dependencies(AtlasObjects GenerateAtlasXml GenerateAtlasObjects)
    add_dependencies(GenerateAtlasObjects GenerateAtlasXml)
endif (ATLAS_GENERATE_OBJECTS)

wf_add_library(AtlasFilters FILTERS_SOURCE_FILES FILTERS_HEADER_FILES)
target_link_libraries(AtlasFilters Atlas)

if (BZIP2_FOUND)
    target_link_libraries(AtlasFilters BZip2::BZip2)
    add_definitions(-DHAVE_BZLIB_H -DHAVE_LIBBZ2)
endif (BZIP2_FOUND)

if (ZLIB_FOUND)
    target_link_libraries(AtlasFilters ZLIB::ZLIB)
    add_definitions(-DHAVE_ZLIB_H -DHAVE_LIBZ)
endif (ZLIB_FOUND)

if (NOT NO_LIBS_INSTALL)

    CHECK_INCLUDE_FILE(unistd.h HAS_UNISTD)
    # The "atlas_convert" tool, only available on systems which provides unistd.h
    if (HAS_UNISTD)
        add_executable(atlas_convert atlas_convert.cpp)
        target_compile_options(atlas_convert PRIVATE ${WF_WARNING_FLAGS})
        target_link_libraries(atlas_convert AtlasCodecs AtlasMessage Atlas)

        install(TARGETS atlas_convert
                DESTINATION ${CMAKE_INSTALL_BINDIR})
    endif ()
endif ()