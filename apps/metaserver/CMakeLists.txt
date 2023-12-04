project(Metaserver)

# Meta data
set(DESCRIPTION "The Worldforge metaserver.")

include(GNUInstallDirs)


add_subdirectory(src)
add_subdirectory(tests)

if (BUILD_METASERVER_SERVER)
    configure_file(metaserver-ng.conf.in metaserver-ng.conf @ONLY)
    configure_file(metaserver-ng.dox.in metaserver-ng.dox @ONLY)
    configure_file(opentsdb-collector.py.in opentsdb-collector.py @ONLY)
    configure_file(mswatchdog.sh.in mswatchdog.sh @ONLY)

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/metaserver-ng.conf DESTINATION ${CMAKE_INSTALL_FULL_SYSCONFDIR})
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/mswatchdog.sh DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/opentsdb-collector.py DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})
endif ()