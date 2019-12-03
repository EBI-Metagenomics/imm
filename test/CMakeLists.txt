function(imm_add_test name)
    add_executable(test_${name} src/${name}.c)
    target_link_libraries(test_${name} PRIVATE IMM::imm)
    add_test(NAME ${name} COMMAND test_${name})
endfunction()

imm_add_test(abc)
imm_add_test(hmm)
imm_add_test(path)
imm_add_test(state)