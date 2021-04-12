add_custom_target(all_tests)
find_package(cass REQUIRED)

function(imm_add_test name srcs)
    add_executable(test_${name} ${srcs})
    add_dependencies(all_tests test_${name})
    target_link_libraries(test_${name} PRIVATE IMM::imm)
    target_link_libraries(test_${name} PUBLIC CASS::cass)
    target_compile_options(test_${name} PRIVATE ${WARNING_FLAGS})
    add_test(NAME ${name} COMMAND test_${name})

    set(TMPDIR "${CMAKE_CURRENT_BINARY_DIR}/test_${name}.tmp")
    add_custom_command(TARGET test_${name} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${TMPDIR}
    )
    target_compile_definitions(test_${name} PUBLIC "TMPDIR=\"${TMPDIR}\"")
endfunction()

imm_add_test(abc abc.c)
imm_add_test(abc_lprob abc_lprob.c)
imm_add_test(hmm hmm.c)
imm_add_test(hmm_io hmm_io.c)
imm_add_test(hmm_likelihood hmm_likelihood.c)
imm_add_test(hmm_viterbi hmm_viterbi.c)
imm_add_test(path path.c)
imm_add_test(perf "perf.c;model2.c")
imm_add_test(seq_table seq_table.c)
imm_add_test(state state.c)
