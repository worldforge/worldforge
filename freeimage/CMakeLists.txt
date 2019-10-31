project(FreeImage)
cmake_minimum_required(VERSION 3.0)

option(BUILD_SHARED_LIBS "Build shared library if true, else build static library" OFF)

include(../conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

#Get the sources from Makefile.srcs
file(STRINGS Makefile.srcs FREEIMAGE_SOURCES REGEX "SRCS = .*")
file(STRINGS Makefile.srcs FREEIMAGE_INCLUDE_DIR REGEX "INCLUDE = .*")

string(REPLACE "SRCS = " "" FREEIMAGE_SOURCES ${FREEIMAGE_SOURCES})
string(REPLACE "./" "" FREEIMAGE_SOURCES ${FREEIMAGE_SOURCES})
string(REPLACE " " ";" FREEIMAGE_SOURCES ${FREEIMAGE_SOURCES})
string(REPLACE "INCLUDE = " "" FREEIMAGE_INCLUDE_DIR ${FREEIMAGE_INCLUDE_DIR})
string(REPLACE "-I" "" FREEIMAGE_INCLUDE_DIR ${FREEIMAGE_INCLUDE_DIR})
string(REPLACE " " ";" FREEIMAGE_INCLUDE_DIR ${FREEIMAGE_INCLUDE_DIR})

add_library(FreeImage
	${FREEIMAGE_SOURCES}
)

target_include_directories(FreeImage PRIVATE
	${FREEIMAGE_INCLUDE_DIR}
)

target_compile_definitions(FreeImage PRIVATE OPJ_STATIC LIBRAW_NODLL LIBRAW_LIBRARY_BUILD NO_LCMS USE_JPIP DISABLE_PERF_MEASUREMENT __ANSI__)
if(BUILD_SHARED_LIBS)
	target_compile_definitions(FreeImage PUBLIC FREEIMAGE_EXPORTS)
else()
	target_compile_definitions(FreeImage PUBLIC FREEIMAGE_LIB)
endif()

target_link_libraries(FreeImage PRIVATE CONAN_LIBS)

if (APPLE)
	set_target_properties(FreeImage PROPERTIES XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH "NO")

	if(IOS)
		set_target_properties(FreeImage PROPERTIES XCODE_ATTRIBUTE_GCC_THUMB_SUPPORT "NO")
	endif()
endif()

# With -std=c++11 (default from GCC 6.1) or higher, GCC fails to compile due to an invalid narrowing conversion
if (CMAKE_COMPILER_IS_GNUCXX)
	target_compile_options(FreeImage PUBLIC -Wno-narrowing -fexceptions -fvisibility=hidden)

	# Ignore deprecated warnings in gcc 7+ regarding throw(...)
	if(NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7)
		target_compile_options(FreeImage PRIVATE -Wno-deprecated)
	endif()
endif()


set(FreeImage_INCLUDE_DIR "${FreeImage_SOURCE_DIR}/Source" CACHE PATH "" FORCE)
set(FreeImage_LIBRARY_DBG FreeImage CACHE STRING "" FORCE)
set(FreeImage_LIBRARY_REL FreeImage CACHE STRING "" FORCE)
mark_as_advanced(FreeImage_INCLUDE_DIR FreeImage_LIBRARY_DBG FreeImage_LIBRARY_REL)
