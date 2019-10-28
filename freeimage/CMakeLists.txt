project(FreeImage)
cmake_minimum_required(VERSION 3.0)

option(BUILD_SHARED_LIBS "Build shared library if true, else build static library" OFF)

include(../conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

file(GLOB FREEIMAGE_SOURCES Source/FreeImage/*.cpp Source/FreeImage/*.c)
file(GLOB FREEIMAGETOOLKIT_SOURCES Source/FreeImageToolkit/*.cpp Source/FreeImageToolkit/*.c)
file(GLOB LIBOPENJPEG_SOURCES Source/LibOpenJPEG/*.cpp Source/LibOpenJPEG/*.c)
file(GLOB_RECURSE LIBJXR_SOURCES Source/LibJXR/*.cpp Source/LibJXR/*.c)
#file(GLOB_RECURSE LIBRAWLITE_SOURCES Source/LibRawLite/*.cpp Source/LibRawLite/*.c)
set(LIBRAWLITE_SOURCES
	Source/LibRawLite/internal/dcraw_common.cpp
	Source/LibRawLite/internal/dcraw_fileio.cpp
	Source/LibRawLite/internal/demosaic_packs.cpp
	Source/LibRawLite/src/libraw_c_api.cpp
	Source/LibRawLite/src/libraw_cxx.cpp
	Source/LibRawLite/src/libraw_datastream.cpp
)
file(GLOB METADATA_SOURCES Source/Metadata/*.cpp Source/Metadata/*.c)

foreach(src ${FREEIMAGE_SOURCES})
	if(src MATCHES PluginJXR.cpp)
		list(REMOVE_ITEM FREEIMAGE_SOURCES "${src}")
	endif()
endforeach()

add_library(FreeImage
	${FREEIMAGE_SOURCES}
	${FREEIMAGETOOLKIT_SOURCES}
	${LIBOPENJPEG_SOURCES}
	#${LIBJXR_SOURCES}
	${LIBRAWLITE_SOURCES}
	${METADATA_SOURCES}
	Source/LibJPEG/transupp.c
	Source/CacheFile.h
	Source/FreeImage.h
	Source/FreeImageIO.h
	Source/Plugin.h
	Source/Quantizers.h
	Source/ToneMapping.h
	Source/Utilities.h
)

# need include directories for FreeImage and all the sub libs
target_include_directories(FreeImage PRIVATE
	.
	Source
	Source/FreeImageToolkit
	Source/LibOpenJPEG
	Source/LibRawLite
	Source/LibJXR/common/include
	Source/LibJXR/jxrgluelib
	Source/LibJXR/image/sys
#	Source/OpenEXR
#	Source/OpenEXR/Half
#	Source/OpenEXR/Iex
#	Source/OpenEXR/IlmImf
#	Source/OpenEXR/IlmThread
#	Source/OpenEXR/Imath
)

target_compile_definitions(FreeImage PRIVATE OPJ_STATIC LIBRAW_NODLL LIBRAW_LIBRARY_BUILD NO_LCMS USE_JPIP)
if(BUILD_SHARED_LIBS)
	target_compile_definitions(FreeImage PUBLIC FREEIMAGE_EXPORTS)
else()
	target_compile_definitions(FreeImage PUBLIC FREEIMAGE_LIB)
endif()

target_link_libraries(FreeImage PRIVATE CONAN_PKG::libjpeg CONAN_PKG::libpng CONAN_PKG::TIFF CONAN_PKG::libwebp CONAN_PKG::openexr CONAN_PKG::zlib)

if (APPLE)
	set_target_properties(FreeImage PROPERTIES XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH "NO")

	if(IOS)
		set_target_properties(FreeImage PROPERTIES XCODE_ATTRIBUTE_GCC_THUMB_SUPPORT "NO")
	endif()
endif()

# With -std=c++11 (default from GCC 6.1) or higher, GCC fails to compile due to an invalid narrowing conversion
if (CMAKE_COMPILER_IS_GNUCXX)
	target_compile_options(FreeImage PUBLIC -Wno-narrowing)

	# Ignore deprecated warnings in gcc 7+ regarding throw(...)
	if(NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7)
		target_compile_options(FreeImage PRIVATE -Wno-deprecated)
	endif()
endif()


set(FreeImage_INCLUDE_DIR "${FreeImage_SOURCE_DIR}/Source" CACHE PATH "" FORCE)
set(FreeImage_LIBRARY_DBG FreeImage CACHE STRING "" FORCE)
set(FreeImage_LIBRARY_REL FreeImage CACHE STRING "" FORCE)
mark_as_advanced(FreeImage_INCLUDE_DIR FreeImage_LIBRARY_DBG FreeImage_LIBRARY_REL)
