add_subdirectory(blake3)

if (NOT NO_LIBS_INSTALL)

    install(TARGETS blake3
            EXPORT "${PROJECT_NAME}Targets"
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

    install(FILES blake3/blake3.h blake3/blake3_impl.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/squall)
endif ()

#We need the "bytesize" external component which resides at the top of the repo. We can recognize that this isn't a normal
#monorepo build by the missing "MONOREPO_BUILD" variable.
#Note that we also need to handle the case of us building a Conan package, by which we've already had Conan copy the
#"bytesize" directory here.
if (NOT MONOREPO_BUILD)
    if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/bytesize)
        message(STATUS "This is not a monorepo build, and the 'bytesize' directory exists as a subdirectory, which it should do if it's a Conan package build.")
        add_subdirectory(bytesize)
    else ()
        message(STATUS "This is not a monorepo build, and the 'bytesize' directory needs to be added from the top of the root directory.")
        add_subdirectory(../../../external/bytesize bytesize)
    endif ()
endif ()