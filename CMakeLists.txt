cmake_minimum_required(VERSION 3.25.1)
project(ssh-client
        VERSION 0.0.1
        LANGUAGES CXX
        HOMEPAGE_URL "https://github.com/khromenokroman/ssh_client")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(GNUInstallDirs)

if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(CMAKE_PREFIX_PATH "/opt/homebrew")
endif ()

configure_file(Version.in Version @ONLY)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBSSH2 REQUIRED libssh2)
find_package(Boost REQUIRED COMPONENTS program_options)

if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    include_directories(${LIBSSH2_INCLUDE_DIRS})
    link_directories(${LIBSSH2_LIBRARY_DIRS} /opt/homebrew/lib)
endif ()

# lib
add_library(${PROJECT_NAME}-lib SHARED
        src/ssh_client.cpp
)
set_target_properties(${PROJECT_NAME}-lib PROPERTIES
        OUTPUT_NAME ${PROJECT_NAME}
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
)
target_include_directories(${PROJECT_NAME}-lib PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${LIBSSH2_INCLUDE_DIRS}
)
target_link_libraries(${PROJECT_NAME}-lib PRIVATE
        ${LIBSSH2_LIBRARIES}
)

# bin
add_executable(${PROJECT_NAME}
        src/main.cpp
)
target_include_directories(${PROJECT_NAME} PRIVATE
        ${Boost_INCLUDE_DIRS}
)
target_link_libraries(${PROJECT_NAME} PRIVATE
        ${PROJECT_NAME}-lib
        Boost::program_options
        ${LIBSSH2_LIBRARIES}
)

# install
install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT runtime
)
install(TARGETS ${PROJECT_NAME}-lib
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT runtime
        NAMELINK_COMPONENT dev
)
install(FILES ${CMAKE_SOURCE_DIR}/include/ssh_client.hpp
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
        COMPONENT dev
)
install(FILES ${CMAKE_SOURCE_DIR}/completion/_ssh-client
        DESTINATION share/zsh/vendor-completions
        COMPONENT runtime
)

# DEB
set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VENDOR "khromenokroman")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_CONTACT "khromenokroman")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "SSH Client(simple)")
set(CPACK_PACKAGE_HOMEPAGE_URL "${PROJECT_HOMEPAGE_URL}")

set(CPACK_GENERATOR "DEB")
set(CPACK_DEB_COMPONENT_INSTALL ON)
set(CPACK_COMPONENTS_ALL runtime dev)

set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "khromenokroman")
set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

set(CPACK_DEBIAN_RUNTIME_PACKAGE_NAME "ssh-client")
set(CPACK_DEBIAN_RUNTIME_PACKAGE_SECTION "utils")
set(CPACK_DEBIAN_RUNTIME_PACKAGE_DESCRIPTION "Simple console SSH client")
set(CPACK_DEBIAN_RUNTIME_PACKAGE_DEPENDS "")

set(CPACK_DEBIAN_DEV_PACKAGE_NAME "ssh-client-dev")
set(CPACK_DEBIAN_DEV_PACKAGE_SECTION "libdevel")
set(CPACK_DEBIAN_DEV_PACKAGE_DESCRIPTION "Development files for ssh-client library")
set(CPACK_DEBIAN_DEV_PACKAGE_DEPENDS "ssh-client (= ${PROJECT_VERSION})")

include(CPack)