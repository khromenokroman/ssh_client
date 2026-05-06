if (ENABLE_BUILD_CLI_TOOL)
    install(TARGETS ${PROJECT_NAME}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            COMPONENT runtime
    )
endif ()

install(TARGETS ${PROJECT_NAME}-lib
        EXPORT ${PROJECT_NAME}Targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT runtime
        NAMELINK_COMPONENT dev
)

install(FILES ${PROJECT_SOURCE_DIR}/include/ssh_client.hpp
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
        COMPONENT dev
)

if (ENABLE_BUILD_CLI_TOOL)
    install(FILES ${PROJECT_SOURCE_DIR}/completion/_ssh-client
            DESTINATION share/zsh/vendor-completions
            COMPONENT runtime
    )
endif ()

set(CMAKE_CONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

install(EXPORT ${PROJECT_NAME}Targets
        FILE ${PROJECT_NAME}Targets.cmake
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION ${CMAKE_CONFIG_INSTALL_DIR}
        COMPONENT dev
)

configure_package_config_file(
        ${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in
        ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
        INSTALL_DESTINATION ${CMAKE_CONFIG_INSTALL_DIR}
)

write_basic_package_version_file(
        ${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
)

install(FILES
        ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
        ${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
        DESTINATION ${CMAKE_CONFIG_INSTALL_DIR}
        COMPONENT dev
)