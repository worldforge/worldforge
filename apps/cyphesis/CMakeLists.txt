cmake_minimum_required(VERSION 3.12)

project(Cyphesis)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/tools/cmake)

include(GNUInstallDirs)
include(GetGitRevisionDescription)
include(CMakeDependentOption)

# Set compiler flags
set(WF_WARNING_FLAGS -Wall -Winit-self -Wcast-qual -Wwrite-strings -Wextra -Wundef -Wmissing-declarations -Wno-unused-parameter -Wshadow -Wno-missing-field-initializers -Wno-long-long)

add_definitions(-DHAVE_CONFIG_H)
add_definitions(-DENABLE_BINRELOC)

include_directories(${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/external ${PROJECT_BINARY_DIR}/generated)

enable_testing()

# Meta data
get_git_head_revision(GIT_REFSPEC GIT_SHA1)

set(DESCRIPTION "The Worldforge server.")
set(SOCKET_DIR ${CMAKE_INSTALL_FULL_LOCALSTATEDIR}/tmp)
set(PACKAGE_BUGREPORT "erik@ogenvik.org")

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
link_libraries(Threads::Threads)

#If we're using Conan this won't set the Python3_EXECUTABLE environment variables as expected, which is why we do that in conanfile.py
find_package(Python3 3.12.2 EXACT COMPONENTS Development Interpreter REQUIRED)

find_package(sigc++-3 3.0.7 EXACT REQUIRED)
find_package(libgcrypt 1.10.3 EXACT REQUIRED)
find_package(Bullet 2.89 EXACT REQUIRED)

find_package(Avahi)
if (AVAHI_FOUND)
    link_directories(${AVAHI_LIBRARY_DIRS})
    include_directories(${AVAHI_INCLUDE_DIR})
    set(HAVE_AVAHI 1)
endif (AVAHI_FOUND)

find_package(PostgreSQL 7.1)
cmake_dependent_option(CYPHESIS_USE_POSTGRES "Use PostgreSQL for storage" FALSE "PostgreSQL_FOUND" FALSE)
if (CYPHESIS_USE_POSTGRES)
    message(STATUS "Compiling with Postgres support.")
    add_compile_definitions(CYPHESIS_USE_POSTGRES)
else ()
    message(STATUS "Compiling without Postgres support (will use SQLite only).")
endif ()


find_package(SQLite3 3.49.1 EXACT REQUIRED)

find_package(readline REQUIRED)
wf_find_boost(system)

set(MEDIAREPO_DIR ${PROJECT_SOURCE_DIR}/mediarepo)
add_compile_definitions("CYPHESIS_RAW_ASSETS_DIRECTORY=\"${MEDIAREPO_DIR}/trunk/assets\"")

find_program(SUBVERSION_CMD NAMES svn)
find_program(RSYNC_CMD NAMES rsync)

#It would be better if Bullet provided a header which specified how it was built...
OPTION(BT_USE_DOUBLE_PRECISION "Using double precision with Bullet. This should match what Bullet has been compiled with." OFF)

if (BT_USE_DOUBLE_PRECISION)
    message(STATUS "Using double precision with Bullet. Make sure that Bullet is compiled with this too.")
    add_definitions(-DBT_USE_DOUBLE_PRECISION=1)
endif ()

#You can specify an exernal installation of the Worlds though WORLDFORGE_WORLDS_PATH environment variable. If that's not specified,
#Worlds data will be installed as specified by the WORLDFORGE_WORLDS_SOURCE_PATH environment variable (which is set by Conan).
if (WORLDFORGE_WORLDS_PATH)
    message(STATUS "An installation of the Worldforge Worlds has been specified through the WORLDFORGE_WORLDS_PATH environment variable: ${WORLDFORGE_WORLDS_PATH}.")
else ()
    set(WORLDFORGE_WORLDS_PATH "${CMAKE_INSTALL_PREFIX}/share/cyphesis/worlds")

    if (WORLDFORGE_WORLDS_SOURCE_PATH)
        install(DIRECTORY ${WORLDFORGE_WORLDS_SOURCE_PATH}/ DESTINATION ${WORLDFORGE_WORLDS_PATH})
        message(STATUS "Added installation of the Worldforge Worlds from '${WORLDFORGE_WORLDS_SOURCE_PATH}' to the 'install' target.")
    else ()
        message(STATUS "No Worldforge Worlds installation has been specified (through the WORLDFORGE_WORLDS_SOURCE_PATH environment variable); we will work on the assumption that there's an installation already at '${WORLDFORGE_WORLDS_PATH}'.")
    endif ()
endif ()

add_custom_target(mediarepo-checkout)
add_custom_target(mediarepo-checkout-full)
if (SUBVERSION_CMD)
    #We'll only checkout the "assets" directory by default, and without any "source" directories.
    add_custom_command(
            TARGET mediarepo-checkout
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "Using Subversion to do sparse checkout https://svn.worldforge.org:886/svn/media/trunk to ${MEDIAREPO_DIR}/trunk, with a subset of files."
            COMMAND ${CMAKE_COMMAND} -E make_directory ${MEDIAREPO_DIR}
            COMMAND [ ! -d ${MEDIAREPO_DIR}/trunk ] && ${SUBVERSION_CMD} checkout https://svn.worldforge.org:886/svn/media/trunk ${MEDIAREPO_DIR}/trunk --depth files || true
            COMMAND cd ${MEDIAREPO_DIR}/trunk && ${SUBVERSION_CMD} list --recursive https://svn.worldforge.org:886/svn/media/trunk | grep assets/ | grep --invert-match /source | xargs ${SUBVERSION_CMD} update --depth empty
    )
    #This will do a full checkout of all assets. Useful if you intend to do development where you edit media.
    add_custom_command(
            TARGET mediarepo-checkout-full
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "Using Subversion to checkout https://svn.worldforge.org:886/svn/media/trunk to ${MEDIAREPO_DIR}/trunk."
            COMMAND ${CMAKE_COMMAND} -E make_directory ${MEDIAREPO_DIR}
            COMMAND ${SUBVERSION_CMD} checkout https://svn.worldforge.org:886/svn/media/trunk ${MEDIAREPO_DIR}/trunk --depth infinity
    )

else (SUBVERSION_CMD)
    add_custom_command(
            TARGET mediarepo-checkout
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "Could not find the Subversion command 'svn'. The target 'mediarepo-checkout' is disabled."
    )
endif (SUBVERSION_CMD)

set(MEDIAREPO_PROCESSED_DIR ${CMAKE_INSTALL_PREFIX}/share/cyphesis/assets)
if (NOT MEDIA_PROCESS_TEXTURE_MAX_SIZE)
    set(MEDIA_PROCESS_TEXTURE_MAX_SIZE 512)
endif ()

add_custom_target(media-process-install)
if (Python3_EXECUTABLE)
    add_custom_command(
            TARGET media-process-install
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "Processing media in '${MEDIAREPO_DIR}/trunk' and placing it in '${MEDIAREPO_PROCESSED_DIR}'."
            COMMAND ${CMAKE_COMMAND} -E echo "NOTE: this will make Cyphesis use the processed media in '${MEDIAREPO_PROCESSED_DIR}' instead of the raw media in '${MEDIAREPO_DIR}/trunk'."
            COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/scripts/make_dist_media.py ${MEDIAREPO_DIR}/trunk ${MEDIAREPO_PROCESSED_DIR} ${MEDIA_PROCESS_TEXTURE_MAX_SIZE}
    )
else (Python3_EXECUTABLE)
    add_custom_command(
            TARGET media-process-install
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "Could not find the Python command 'python'. The target 'mediarepo-process' is disabled."
    )
endif (Python3_EXECUTABLE)

add_custom_target(media-download-rsync)
if (RSYNC_CMD)
    add_custom_command(
            TARGET media-download-rsync
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "I will now use rsync to install the assets from amber.worldforge.org into ${MEDIAREPO_PROCESSED_DIR}."
            COMMAND ${CMAKE_COMMAND} -E make_directory ${MEDIAREPO_PROCESSED_DIR}
            COMMAND rsync -rtzu amber.worldforge.org::worldforge-media ${MEDIAREPO_PROCESSED_DIR}
            COMMAND ${CMAKE_COMMAND} -E echo "Done."
    )
else (RSYNC_CMD)
    add_custom_command(
            TARGET media-download-rsync
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "Could not find the Rsync command. The target 'assets-download' is disabled."
    )
endif (RSYNC_CMD)

##TODO: check for binreloc?

#We'll use xmllint for validating schemas of some of our xml files.
find_program(XMLLINT xmllint)

find_package(spdlog REQUIRED)

# Add a "check" target, which builds and runs the tests.
#add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} -E Benchmark)

# Add a "benchmark" target, which builds and runs the benchmarks.
#add_custom_target(benchmark COMMAND ${CMAKE_CTEST_COMMAND} -R Benchmark)

configure_file(tools/Version.tmpl.h generated/Version.h @ONLY)


add_subdirectory(src)
add_subdirectory(external)
add_subdirectory(data)
add_subdirectory(tools)
add_subdirectory(docs)
#Only build tests if the "check" target is used.
add_subdirectory(tests EXCLUDE_FROM_ALL)

configure_file(docs/Doxyfile.in Doxyfile @ONLY)
message(STATUS "Installing snapcraft.yaml file into build directory. Copy this one to the installation directory if you want to build snaps, or preferably use the 'snap-build-cyphesis' build target.")
configure_file(tools/snapcraft.tmpl.yaml snap/snapcraft.yaml @ONLY)

install(FILES README.md COPYING AUTHORS THANKS DESTINATION ${CMAKE_INSTALL_DATADIR}/doc/cyphesis)
install(DIRECTORY DESTINATION ${SOCKET_DIR})

if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    #On systems with Snapcraft this command will build a snap, otherwise one has to do some extra copying of the "snap" folder.
    add_custom_target(snap-build-cyphesis)
    add_custom_command(
            TARGET snap-build-cyphesis
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "First I need to copy the Python scripts needed for the base Python installation from ${PYTHONHOME}/lib."
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_INSTALL_PREFIX}/lib
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${PYTHONHOME}/lib ${CMAKE_INSTALL_PREFIX}/lib

            COMMAND ${CMAKE_COMMAND} -E echo "Then I need to copy the Worldforge Worlds data from ${WORLDFORGE_WORLDS_SOURCE_PATH}."
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_INSTALL_PREFIX}/share/worldforge
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${WORLDFORGE_WORLDS_SOURCE_PATH} ${CMAKE_INSTALL_PREFIX}/share/worldforge/worlds

            COMMAND ${CMAKE_COMMAND} -E echo "I will now copy the snapfile to the installation directory and run 'snapcraft'."
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_INSTALL_PREFIX}/snap
            COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/snap/snapcraft.yaml ${CMAKE_INSTALL_PREFIX}/snap/snapcraft.yaml
            COMMAND ${SNAPCRAFT_COMMAND}
            WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}
    )
endif ()

## Doxygen support, exports a "dox" target.

find_package(Doxygen)

if (DOXYGEN_FOUND)

    set(DOXYGEN_OUTPUT doc)

    add_custom_command(
            OUTPUT ${DOXYGEN_OUTPUT}
            COMMAND ${CMAKE_COMMAND} -E echo_append "Building API Documentation..."
            COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
            COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile-python
            COMMAND ${CMAKE_COMMAND} -E echo "Done."
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile-python
    )

    #    add_custom_target(dox DEPENDS ${DOXYGEN_OUTPUT})

endif (DOXYGEN_FOUND)

add_custom_command(
        OUTPUT ChangeLog
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tools/support/generate-ChangeLog.sh ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} cce66ef4a196a4871ebaebe82cd91c8dd733be13
)
#add_custom_target(changelog DEPENDS ChangeLog)

