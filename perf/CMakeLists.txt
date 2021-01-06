function(imm_add_perf name)
    add_executable(perf_${name} src/1thread_viterbi.c)
    target_link_libraries(perf_${name} PRIVATE IMM::imm)
    target_compile_options(perf_${name} PRIVATE ${WARNING_FLAGS})
endfunction()

imm_add_perf(1thread_viterbi)
