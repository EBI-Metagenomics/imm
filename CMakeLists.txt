cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(imm VERSION 0.0.1 LANGUAGES C)

# Generate compile_commands.json file
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(imm
    src/abc.c
    src/ascii.c
    src/dp.c
    src/hmm.c
    src/lprob.c
    src/matrix.c
    src/mm_state.c
    src/mm_state_sort.c
    src/mm_trans.c
    src/mute_state.c
    src/normal_state.c
    src/path.c
    src/report.c
    src/state.c
    src/state_idx.c
    src/step.c
    src/table_state.c
)

target_include_directories(imm
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
set_target_properties(imm PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(imm PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})
target_compile_features(imm PRIVATE c_std_11)
target_compile_definitions(imm PRIVATE IMM_API_EXPORTS)
target_compile_options(imm PRIVATE $<$<C_COMPILER_ID:AppleClang>:-Wall -Wextra
    -pedantic -Wstrict-prototypes -Wshadow -Wconversion -Wmissing-prototypes
    -Wno-unused-parameter -Wno-empty-translation-unit>)

##############################################################################
# Installation instructions

include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

install(TARGETS imm
    EXPORT imm-targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Export the targets to a script
install(EXPORT imm-targets
    FILE imm-targets.cmake
    NAMESPACE IMM::
    DESTINATION ${INSTALL_CONFIGDIR}
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(imm-config-version.cmake
                                 VERSION ${PROJECT_VERSION}
                                 COMPATIBILITY AnyNewerVersion)

configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/imm-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/imm-config.cmake
    INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/imm-config-version.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/imm-config.cmake
    DESTINATION ${INSTALL_CONFIGDIR}
)

##############################################################################
export(EXPORT imm-targets FILE ${CMAKE_CURRENT_BINARY_DIR}/imm-targets.cmake NAMESPACE IMM::)

# Export the package for use from the build-tree
export(PACKAGE ${PROJECT_NAME})

enable_testing()
add_subdirectory(test)
