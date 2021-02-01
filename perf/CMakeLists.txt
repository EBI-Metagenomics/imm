function(imm_add_perf name)
    add_executable(perf_${name} src/${name}.c)
    target_link_libraries(perf_${name} PRIVATE IMM::imm)
    target_compile_options(perf_${name} PRIVATE ${WARNING_FLAGS})
endfunction()

imm_add_perf(1thread_viterbi)

file(COPY run-perf DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
file(COPY show-perf DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
