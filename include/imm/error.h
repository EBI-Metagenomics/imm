#ifndef ERROR_H
#define ERROR_H

#include "imm/export.h"

enum imm_error_code
{
    IMM_SUCCESS = 0,
    IMM_FAILURE,
    IMM_OUTOFMEM,
    IMM_ILLEGALARG,
    IMM_IOERROR,
    IMM_NOTIMPLEMENTED,
    IMM_RUNTIMEERROR,
    IMM_PARSEERROR,
};

IMM_API void imm_die(enum imm_error_code level, char const *msg)
    __attribute__((noreturn));

#endif
