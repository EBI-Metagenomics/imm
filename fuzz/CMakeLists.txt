add_custom_target(all_fuzzs)

function(imm_add_fuzz name srcs)
    add_executable(${name} ${srcs})
    add_dependencies(all_fuzzs ${name})
    target_link_libraries(${name} PRIVATE IMM::imm)
    target_compile_options(${name} PRIVATE ${WARNING_FLAGS})
    target_compile_features(${name} PRIVATE c_std_11)

    add_custom_command(
        TARGET ${name} POST_BUILD
        COMMAND ${name} -max_len=512 -seed=1 -runs=1000000 -verbosity=0
        VERBATIM)
endfunction()

imm_add_fuzz(abc_fuzz abc_fuzz.c)
imm_add_fuzz(dp_fuzz dp_fuzz.c)
