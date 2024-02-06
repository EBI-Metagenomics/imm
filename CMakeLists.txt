cmake_minimum_required(VERSION 3.20.2 FATAL_ERROR)
project(imm VERSION 5.4.4 LANGUAGES C)

include(cmake/sanitizers.cmake)
include(cmake/warnings.cmake)
include(cmake/CPM.cmake)

cpmaddpackage("gh:EBI-Metagenomics/lip@0.5.4")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS OFF)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
  set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

file(GLOB IMM_SOURCES "*.c")
file(GLOB IMM_HEADERS "*.h")
list(FILTER IMM_SOURCES EXCLUDE REGEX "test_.*")
list(FILTER IMM_HEADERS EXCLUDE REGEX "test_.*")
list(FILTER IMM_SOURCES EXCLUDE REGEX "benchmark_.*")

add_library(imm ${IMM_SOURCES})
add_library(IMM::imm ALIAS imm)
target_link_libraries(imm PRIVATE LIP::lip)
target_link_libraries(imm INTERFACE $<$<BOOL:${UNIX}>:m>)

file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/imm)
file(COPY ${IMM_HEADERS} DESTINATION ${PROJECT_BINARY_DIR}/imm)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(imm PUBLIC -DIMM_STATIC_DEFINE)
endif()

target_include_directories(
  imm
  PUBLIC $<INSTALL_INTERFACE:include>
         $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>
  PRIVATE ${PROJECT_SOURCE_DIR}
)

set_target_properties(imm PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(imm PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

install(
  TARGETS imm
  EXPORT imm-targets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
install(DIRECTORY ${PROJECT_BINARY_DIR}/imm DESTINATION include)

export(
  TARGETS imm
  NAMESPACE IMM::
  FILE imm-targets.cmake)
install(
  EXPORT imm-targets
  NAMESPACE IMM::
  DESTINATION lib/cmake/imm)

install(
  FILES ${PROJECT_SOURCE_DIR}/README.md ${PROJECT_SOURCE_DIR}/LICENSE.md
  DESTINATION share/docs/imm)

include(CMakePackageConfigHelpers)
set(PROJECT_CONFIG ${PROJECT_BINARY_DIR}/imm-config.cmake)
set(VERSION_CONFIG ${PROJECT_BINARY_DIR}/imm-config-version.cmake)
configure_package_config_file(imm-config.cmake.in ${PROJECT_CONFIG}
                              INSTALL_DESTINATION lib/cmake/imm)
write_basic_package_version_file(${VERSION_CONFIG}
                                 COMPATIBILITY SameMajorVersion)
install(FILES ${PROJECT_CONFIG} ${VERSION_CONFIG} DESTINATION lib/cmake/imm)

if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
  enable_testing()
  file(GLOB IMM_TEST_SOURCES "test_*.c")
  foreach(source ${IMM_TEST_SOURCES})
    get_filename_component(x ${source} NAME_WE)
    add_executable(${x} ${source})
    target_link_libraries(${x} PRIVATE imm)
    target_link_libraries(${x} PRIVATE LIP::lip)
    add_test(NAME ${x} COMMAND ${x} WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
  endforeach()

  file(GLOB IMM_BENCHMARK_SOURCES "benchmark_*.c")
  foreach(source ${IMM_BENCHMARK_SOURCES})
    get_filename_component(x ${source} NAME_WE)
    add_executable(${x} ${source})
    target_link_libraries(${x} PRIVATE imm)
  endforeach()
endif()
