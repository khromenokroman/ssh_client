cmake_minimum_required(VERSION 3.25.1)
project(ssh-client
        VERSION 0.2.0
        LANGUAGES CXX
        HOMEPAGE_URL "https://github.com/khromenokroman/ssh_client")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(options)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

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
        EXPORT_NAME ${PROJECT_NAME}
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
)
target_include_directories(${PROJECT_NAME}-lib PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>
        ${LIBSSH2_INCLUDE_DIRS}
)
target_link_libraries(${PROJECT_NAME}-lib PRIVATE
        ${LIBSSH2_LIBRARIES}
)
if (SSH_CLIENT_BUILD_CLI)
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
endif ()

if (SSH_CLIENT_BUILD_INSTALL)
    add_subdirectory(install)
endif ()
if (SSH_CLIENT_BUILD_DEB)
    add_subdirectory(deb)
endif ()