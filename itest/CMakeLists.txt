cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(runimm VERSION 0.0.1 LANGUAGES C)

find_package(imm 0.0.1 REQUIRED)

add_executable(runimm runimm.c)
target_link_libraries(runimm PRIVATE IMM::imm)
