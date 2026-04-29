option(SSH_CLIENT_BUILD_CLI "Build ssh-client command line tool" OFF)
option(SSH_CLIENT_BUILD_INSTALL "Build install rules" OFF)
option(SSH_CLIENT_BUILD_DEB "Build deb package rules" OFF)

if (SSH_CLIENT_BUILD_DEB)
    set(SSH_CLIENT_BUILD_INSTALL ON CACHE BOOL "Build install rules" FORCE)
endif ()