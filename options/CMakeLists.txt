option(SSH_CLIENT_BUILD_CLI "Build ssh-client command line tool" OFF)
option(SSH_CLIENT_BUILD_INSTALL "Build install rules" OFF)
option(SSH_CLIENT_BUILD_DEB "Build deb package rules" OFF)

if (SSH_CLIENT_BUILD_DEB)
    set(SSH_CLIENT_BUILD_INSTALL ON CACHE BOOL "Build install rules" FORCE)
endif ()

message(STATUS "ssh-client build configuration:")
message(STATUS "  Library:       ON (always built)")
message(STATUS "  CLI tool:      ${SSH_CLIENT_BUILD_CLI}")
message(STATUS "  Install rules: ${SSH_CLIENT_BUILD_INSTALL}")
message(STATUS "  DEB package:   ${SSH_CLIENT_BUILD_DEB}")