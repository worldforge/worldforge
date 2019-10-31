# see https://cmake.org/cmake/help/v3.11/module/FindBoost.html

string(REPLACE "," ";" BOOST_CYCLE_GROUP_A
    "${CONAN_USER_BOOST_CYCLE_GROUP_A_lib_short_names}"
)

string(REPLACE "," ";" BOOST_CYCLE_GROUP_B
    "${CONAN_USER_BOOST_CYCLE_GROUP_B_lib_short_names}"
)

string(REPLACE "," ";" BOOST_CYCLE_GROUP_C
    "${CONAN_USER_BOOST_CYCLE_GROUP_C_lib_short_names}"
)

string(REPLACE "," ";" BOOST_CYCLE_GROUP_D
    "${CONAN_USER_BOOST_CYCLE_GROUP_D_lib_short_names}"
)

set(CYCLE_GROUPS
    BOOST_CYCLE_GROUP_A
    BOOST_CYCLE_GROUP_B
    BOOST_CYCLE_GROUP_C
    BOOST_CYCLE_GROUP_D
)

set(COMPONENT_PROPERTIES 
    INTERFACE_LINK_LIBRARIES
    INTERFACE_INCLUDE_DIRECTORIES
    INTERFACE_COMPILE_DEFINITIONS
    INTERFACE_COMPILE_OPTIONS
)

function(set_name_if_group target_name)
    set(target_or_group_name ${target_name} PARENT_SCOPE)
    foreach(group_name ${CYCLE_GROUPS})
        LIST(FIND BOOST_${group_name} ${target_name} FOUND_INDEX)
        if((${FOUND_INDEX} GREATER 0) OR (${FOUND_INDEX} EQUAL 0))
            set(target_or_group_name ${group_name} PARENT_SCOPE)
            break()
        endif()
    endforeach()
endfunction()

message(STATUS "Bincrafter's custom FindBoost.cmake")

function(libraries_to_abs libraries libdir libraries_abs_path)
    foreach(_LIBRARY_NAME ${libraries})
        unset(FOUND_LIBRARY CACHE)
        find_library(FOUND_LIBRARY NAME ${_LIBRARY_NAME} PATHS ${libdir}
                     NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)
        if (FOUND_LIBRARY)
            set(FULLPATH_LIBS ${FULLPATH_LIBS} ${FOUND_LIBRARY})
        else()
            message(STATUS "library ${_LIBRARY_NAME} wasn't found in ${libdir}")
            set(FULLPATH_LIBS ${FULLPATH_LIBS} ${_LIBRARY_NAME})
        endif()
    endforeach()
    unset(FOUND_LIBRARY CACHE)
    set(${libraries_abs_path} ${FULLPATH_LIBS} PARENT_SCOPE)
endfunction()

set(Boost_ALL_COMPONENTS
accumulators
algorithm
align
any
array
asio
assert
assign
atomic
beast
bimap
bind
callable_traits
chrono
circular_buffer
compatibility
compute
concept_check
config
container
container_hash
context
contract
conversion
convert
core
coroutine
coroutine2
crc
cycle_group_a
cycle_group_b
cycle_group_c
cycle_group_d
date_time
detail
disjoint_sets
dll
dynamic_bitset
endian
exception
fiber
filesystem
flyweight
foreach
format
function
function_types
functional
fusion
geometry
gil
graph
graph_parallel
hana
heap
hof
icl
integer
interprocess
intrusive
io
iostreams
iterator
lambda
lexical_cast
local_function
locale
lockfree
log
logic
math
metaparse
move
mp11
mpl
msm
multi_array
multi_index
multiprecision
numeric_conversion
numeric_interval
numeric_odeint
numeric_ublas
optional
parameter
parameter_python
phoenix
poly_collection
polygon
pool
predef
preprocessor
process
program_options
property_map
property_tree
proto
ptr_container
python
qvm
random
range
ratio
rational
regex
safe_numerics
scope_exit
serialization
signals2
smart_ptr
sort
spirit
stacktrace
statechart
static_assert
system
test
thread
throw_exception
timer
tokenizer
tti
tuple
type_erasure
type_index
type_traits
typeof
units
unordered
utility
uuid
variant
vmd
wave
winapi
xpressive
yap
)

# set Boost_INCLUDE_DIRS
foreach(component ${Boost_ALL_COMPONENTS})
    string(TOUPPER ${component} component_upper)
    if (DEFINED CONAN_BOOST_${component_upper}_ROOT)
        set_name_if_group(${component}) # Creates ${target_or_group_name}
        string(TOUPPER ${target_or_group_name} target_or_group_name_upper)

        set(Boost_INCLUDE_DIRS "${Boost_INCLUDE_DIRS};${CONAN_INCLUDE_DIRS_BOOST_${target_or_group_name_upper}}")

    endif()
endforeach()

if(NOT TARGET Boost::boost)
    add_library(Boost::boost INTERFACE IMPORTED)
    set_property(TARGET Boost::boost PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIRS})
endif()

if(NOT TARGET Boost::diagnostic_definitions)
    add_library(Boost::diagnostic_definitions INTERFACE IMPORTED)
    if(WIN32)
        set(Boost_LIB_DIAGNOSTIC_DEFINITIONS "-DBOOST_LIB_DIAGNOSTIC")
        set_target_properties(Boost::diagnostic_definitions PROPERTIES INTERFACE_COMPILE_DEFINITIONS "BOOST_LIB_DIAGNOSTIC")
    endif()
endif()

if(NOT TARGET Boost::disable_autolinking)
    add_library(Boost::disable_autolinking INTERFACE IMPORTED)
    if(WIN32)
        set_target_properties(Boost::disable_autolinking PROPERTIES INTERFACE_COMPILE_DEFINITIONS "BOOST_ALL_NO_LIB")
    endif()
endif()

if(NOT TARGET Boost::dynamic_linking)
    add_library(Boost::dynamic_linking INTERFACE IMPORTED)
    if(WIN32)
        set_target_properties(Boost::dynamic_linking PROPERTIES INTERFACE_COMPILE_DEFINITIONS "BOOST_ALL_DYN_LINK")
    endif()
endif()

foreach(component ${Boost_FIND_COMPONENTS})
    string(TOUPPER ${component} component_upper)
    set_name_if_group(${component}) # Creates ${target_or_group_name}
    string(TOUPPER ${target_or_group_name} target_or_group_name_upper)

    set(conan_target CONAN_PKG::boost_${target_or_group_name})
    set(boost_target Boost::${component})

    if(DEFINED CONAN_LIBS_BOOST_${component_upper})
        libraries_to_abs("${CONAN_LIBS_BOOST_${component_upper}}"
            "${CONAN_LIB_DIRS_BOOST_${component_upper}}"
            CONAN_LIBS_BOOST_${component_upper}_ABS)
        set(Boost_LIBRARIES "${Boost_LIBRARIES};${CONAN_LIBS_BOOST_${component_upper}_ABS}")
    endif()

    if(TARGET ${conan_target})
        set(Boost_${component_upper}_LIBRARY ${conan_target})
    else()
        set(Boost_${component_upper}_LIBRARY ${CONAN_LIBS_BOOST_${component_upper}_ABS})
    endif()

    set(Boost_LIBRARY_DIRS "${Boost_LIBRARY_DIRS};${CONAN_LIB_DIRS_BOOST_${target_or_group_name_upper}}")

    set(Boost_${component_upper}_FOUND TRUE)

    message(STATUS "Boost_${component_upper}_FOUND: ${Boost_${component_upper}_FOUND}")
    if (DEFINED Boost_${component_upper}_LIBRARY)
        message(STATUS "Boost_${component_upper}_LIBRARY: ${Boost_${component_upper}_LIBRARY}")
    endif()

    if(NOT TARGET ${boost_target})
        add_library("${boost_target}" INTERFACE IMPORTED)

        if(TARGET ${conan_target})
            set_property(TARGET ${boost_target} PROPERTY INTERFACE_LINK_LIBRARIES ${conan_target})
        else()
            if(DEFINED CONAN_LIBS_BOOST_${component_upper})
                libraries_to_abs("${CONAN_LIBS_BOOST_${component_upper}}"
                    "${CONAN_LIB_DIRS_BOOST_${component_upper}}"
                    CONAN_LIBS_BOOST_${component_upper}_ABS)
            endif()
            if(DEFINED CONAN_LIBS_BOOST_${component_upper}_RELEASE)
                libraries_to_abs("${CONAN_LIBS_BOOST_${component_upper}_RELEASE}"
                    "${CONAN_LIB_DIRS_BOOST_${component_upper}}"
                    CONAN_LIBS_BOOST_${component_upper}_RELEASE_ABS)
            endif()
            if(DEFINED CONAN_LIBS_BOOST_${component_upper}_DEBUG)
                libraries_to_abs("${CONAN_LIBS_BOOST_${component_upper}_DEBUG}"
                    "${CONAN_LIB_DIRS_BOOST_${component_upper}}"
                    CONAN_LIBS_BOOST_${component_upper}_DEBUG_ABS)
            endif()
            set_property(TARGET ${boost_target} PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                ${CONAN_INCLUDE_DIRS_BOOST_${target_or_group_name_upper}}
                $<$<CONFIG:Release>:${CONAN_INCLUDE_DIRS_BOOST_${target_or_group_name_upper}_RELEASE}>
                $<$<CONFIG:RelWithDebInfo>:${CONAN_INCLUDE_DIRS_BOOST_${target_or_group_name_upper}_RELEASE}>
                $<$<CONFIG:MinSizeRel>:${CONAN_INCLUDE_DIRS_BOOST_${target_or_group_name_upper}_RELEASE}>
                $<$<CONFIG:Debug>:${CONAN_INCLUDE_DIRS_LBOOST_${target_or_group_name_upper}_DEBUG}>)

            # NOTE: component_upper instead of target_or_group_name_upper is intentional
            set_property(TARGET ${boost_target} PROPERTY INTERFACE_LINK_LIBRARIES
                ${CONAN_PACKAGE_TARGETS_BOOST_${component_upper}}
                ${CONAN_LIBS_BOOST_${component_upper}_ABS}
                ${CONAN_SHARED_LINKER_FLAGS_BOOST_${component_upper}_LIST}
                ${CONAN_EXE_LINKER_FLAGS_BOOST_${component_upper}_LIST}
                $<$<CONFIG:Release>:
                ${CONAN_PACKAGE_TARGETS_BOOST_${component_upper}_RELEASE}
                ${CONAN_LIBS_BOOST_${component_upper}_RELEASE_ABS}
                ${CONAN_SHARED_LINKER_FLAGS_BOOST_${component_upper}_RELEASE_LIST}
                ${CONAN_EXE_LINKER_FLAGS_BOOST_${component_upper}_RELEASE_LIST}>
                $<$<CONFIG:RelWithDebInfo>:
                ${CONAN_PACKAGE_TARGETS_BOOST_${component_upper}_RELEASE}
                ${CONAN_LIBS_BOOST_${component_upper}_RELEASE_ABS}
                ${CONAN_SHARED_LINKER_FLAGS_BOOST_${component_upper}_RELEASE_LIST}
                ${CONAN_EXE_LINKER_FLAGS_BOOST_${component_upper}_RELEASE_LIST}>
                $<$<CONFIG:MinSizeRel>:
                ${CONAN_PACKAGE_TARGETS_BOOST_${component_upper}_RELEASE}
                ${CONAN_LIBS_BOOST_${component_upper}_RELEASE_ABS}
                ${CONAN_SHARED_LINKER_FLAGS_BOOST_${component_upper}_RELEASE_LIST}
                ${CONAN_EXE_LINKER_FLAGS_BOOST_${component_upper}_RELEASE_LIST}>
                $<$<CONFIG:Debug>:${CONAN_LIBS_BOOST_${component_upper}_DEBUG_ABS}
                ${CONAN_PACKAGE_TARGETS_BOOST_${component_upper}_DEBUG}
                ${CONAN_SHARED_LINKER_FLAGS_BOOST_${component_upper}_DEBUG_LIST}
                ${CONAN_EXE_LINKER_FLAGS_BOOST_${component_upper}_DEBUG_LIST}>
                )

            set_property(TARGET ${boost_target} PROPERTY INTERFACE_COMPILE_DEFINITIONS
                ${CONAN_COMPILE_DEFINITIONS_BOOST_${target_or_group_name_upper}}
                $<$<CONFIG:Release>:${CONAN_COMPILE_DEFINITIONS_BOOST_${target_or_group_name_upper}_RELEASE}>
                $<$<CONFIG:RelWithDebInfo>:${CONAN_COMPILE_DEFINITIONS_BOOST_${target_or_group_name_upper}_RELEASE}>
                $<$<CONFIG:MinSizeRel>:${CONAN_COMPILE_DEFINITIONS_BOOST_${target_or_group_name_upper}_RELEASE}>
                $<$<CONFIG:Debug>:${CONAN_COMPILE_DEFINITIONS_BOOST_${target_or_group_name_upper}_DEBUG}>)

            set_property(TARGET ${boost_target} PROPERTY INTERFACE_COMPILE_OPTIONS
                ${CONAN_C_FLAGS_BOOST_${target_or_group_name_upper}_LIST} ${CONAN_CXX_FLAGS_BOOST_${target_or_group_name_upper}_LIST}
                $<$<CONFIG:Release>:${CONAN_C_FLAGS_BOOST_${target_or_group_name_upper}_RELEASE_LIST} ${CONAN_CXX_FLAGS_BOOST_${target_or_group_name_upper}_RELEASE_LIST}>
                $<$<CONFIG:RelWithDebInfo>:${CONAN_C_FLAGS_BOOST_${target_or_group_name_upper}_RELEASE_LIST} ${CONAN_CXX_FLAGS_BOOST_${target_or_group_name_upper}_RELEASE_LIST}>
                $<$<CONFIG:MinSizeRel>:${CONAN_C_FLAGS_BOOST_${target_or_group_name_upper}_RELEASE_LIST} ${CONAN_CXX_FLAGS_BOOST_${target_or_group_name_upper}_RELEASE_LIST}>
                $<$<CONFIG:Debug>:${CONAN_C_FLAGS_BOOST_${target_or_group_name_upper}_DEBUG_LIST}  ${CONAN_CXX_FLAGS_BOOST_${target_or_group_name_upper}_DEBUG_LIST}>)
        endif()
    endif()
endforeach()

# extract boost version from boost/version.hpp (boost config)
set(Boost_VERSION 0)
set(Boost_LIB_VERSION "")
if(DEFINED CONAN_INCLUDE_DIRS_BOOST_CONFIG)
  # modular boost packages (bincrafters)
  set(_boost_CONFIG_DIR "${CONAN_INCLUDE_DIRS_BOOST_CONFIG}")
else()
  # monolithic boost package (conan-center)
  set(_boost_CONFIG_DIR "${CONAN_INCLUDE_DIRS_BOOST}")
endif()
file(STRINGS "${_boost_CONFIG_DIR}/boost/version.hpp" _boost_VERSION_HPP_CONTENTS REGEX "#define BOOST_(LIB_)?VERSION ")
unset(_boost_CONFIG_DIR)
set(_Boost_VERSION_REGEX "([0-9]+)")
set(_Boost_LIB_VERSION_REGEX "\"([0-9_]+)\"")
foreach(v VERSION LIB_VERSION)
if("${_boost_VERSION_HPP_CONTENTS}" MATCHES "#define BOOST_${v} ${_Boost_${v}_REGEX}")
  set(Boost_${v} "${CMAKE_MATCH_1}")
endif()
endforeach()
unset(_boost_VERSION_HPP_CONTENTS)

math(EXPR Boost_MAJOR_VERSION "${Boost_VERSION} / 100000")
math(EXPR Boost_MINOR_VERSION "${Boost_VERSION} / 100 % 1000")
math(EXPR Boost_SUBMINOR_VERSION "${Boost_VERSION} % 100")

set(Boost_FOUND TRUE)

message(STATUS "Boost_FOUND: ${Boost_FOUND}")
message(STATUS "Boost_VERSION: ${Boost_VERSION}")
message(STATUS "Boost_LIB_VERSION: ${Boost_LIB_VERSION}")
message(STATUS "Boost_MAJOR_VERSION: ${Boost_MAJOR_VERSION}")
message(STATUS "Boost_MINOR_VERSION: ${Boost_MINOR_VERSION}")
message(STATUS "Boost_SUBMINOR_VERSION: ${Boost_SUBMINOR_VERSION}")
message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}")
message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
