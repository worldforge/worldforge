project ( lua C )
cmake_minimum_required ( VERSION 2.8 )

option(BUILD_SHARED_LIBS "Build shared library if true, else build static library" OFF)

include(conanbuildinfo.cmake)
conan_basic_setup()

include_directories ( src ${CMAKE_CURRENT_BINARY_DIR} )
set ( SRC_CORE src/lapi.c src/lcode.c src/ldebug.c src/ldo.c src/ldump.c src/lfunc.c src/lgc.c src/llex.c
  src/lmem.c src/lobject.c src/lopcodes.c src/lparser.c src/lstate.c src/lstring.c src/ltable.c
  src/ltm.c src/lundump.c src/lvm.c src/lzio.c )
set ( SRC_LIB src/lauxlib.c src/lbaselib.c src/ldblib.c src/liolib.c
  src/lmathlib.c src/loslib.c src/lstrlib.c src/ltablib.c src/loadlib.c src/linit.c )

set ( SRC_LUA src/lua.c )
set ( SRC_LUAC src/luac.c src/print.c )

add_library ( liblua ${SRC_CORE} ${SRC_LIB} )
set_target_properties ( liblua PROPERTIES OUTPUT_NAME lua5.1 )
target_link_libraries ( liblua m )


if(BUILD_SHARED_LIBS)
    target_compile_definitions ( liblua PRIVATE _CRT_SECURE_NO_WARNINGS LUA_BUILD_AS_DLL )
endif()

add_executable ( lua ${SRC_LUA} )
target_link_libraries ( lua liblua )

add_executable ( luac ${SRC_LUAC} )
target_link_libraries ( luac liblua )
