option(ENABLE_BUILD_CLI_TOOL "Build ssh-client command line tool" OFF)

message(STATUS "ssh-client build configuration:")
message(STATUS "  Library:       ON (always built)")
message(STATUS "  CLI tool:      ${ENABLE_BUILD_CLI_TOOL}")