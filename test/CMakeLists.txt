add_custom_target(all_tests)
find_package(cass REQUIRED)

function(imm_add_test name)
    add_executable(test_${name} src/${name}.c src/model1.c src/model2.c)
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

imm_add_test(abc)
imm_add_test(abc_lprob)
imm_add_test(hmm)
imm_add_test(hmm_io)
imm_add_test(hmm_likelihood)
imm_add_test(hmm_viterbi)
imm_add_test(path)
imm_add_test(perf)
imm_add_test(seq_table)
imm_add_test(state)
