function(GetIMMVersion _header _major _minor _patch)
    # Read file content
    file(READ ${_header} CONTENT)

    string(REGEX MATCH ".*define IMM_VERSION_MAJOR *([0-9]+)" VERSION_REGEX "${CONTENT}")
    set(${_major} ${CMAKE_MATCH_1} PARENT_SCOPE)

    string(REGEX MATCH ".*define IMM_VERSION_MINOR *([0-9]+)" VERSION_REGEX "${CONTENT}")
    set(${_minor} ${CMAKE_MATCH_1} PARENT_SCOPE)

    string(REGEX MATCH ".*define IMM_VERSION_PATCH *([0-9]+)" VERSION_REGEX "${CONTENT}")
    set(${_patch} ${CMAKE_MATCH_1} PARENT_SCOPE)
endfunction()
