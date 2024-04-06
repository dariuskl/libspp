include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

install(TARGETS libspp libserialport EXPORT libspp-targets FILE_SET hpps)

write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/libspp-config-version.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
)

install(EXPORT libspp-targets
        FILE libspp-config.cmake
        NAMESPACE spp::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/libspp
)

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/libspp-config-version.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/libspp
)
