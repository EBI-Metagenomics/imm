add_custom_target(all_tests)
find_package(cass REQUIRED)

function(imm_add_test name srcs)
    add_executable(${name} ${srcs})
    add_dependencies(all_tests ${name})
    target_link_libraries(${name} PRIVATE IMM::imm)
    target_link_libraries(${name} PUBLIC CASS::cass)
    target_compile_options(${name} PRIVATE ${WARNING_FLAGS})
    target_compile_features(${name} PRIVATE c_std_11)
    add_test(NAME ${name} COMMAND ${name})

    set(TMPDIR "${CMAKE_CURRENT_BINARY_DIR}/${name}.tmp")
    add_custom_command(TARGET ${name} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${TMPDIR}
    )
    target_compile_definitions(${name} PUBLIC "TMPDIR=\"${TMPDIR}\"")
endfunction()

imm_add_test(abc abc.c)
imm_add_test(abc_lprob abc_lprob.c)
imm_add_test(hmm hmm.c)
imm_add_test(hmm_io hmm_io.c)
imm_add_test(hmm_likelihood hmm_likelihood.c)
imm_add_test(hmm_viterbi hmm_viterbi.c)
imm_add_test(path path.c)
imm_add_test(perf "perf.c;model.c")
imm_add_test(seq_table seq_table.c)
imm_add_test(state state.c)
