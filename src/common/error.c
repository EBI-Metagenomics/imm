#include "common/error.h"
#include "imm/error.h"

static char const *__msg[] = {
    [IMM_SUCCESS] = "success",          [IMM_FAILURE] = "failure",
    [IMM_OUTOFMEM] = "out of memory",   [IMM_ILLEGALARG] = "illegal argument",
    [IMM_IOERROR] = "i/o error",        [IMM_PARSEERROR] = "parse error",
    [IMM_RUNTIMEERROR] "runtime error",
};

int __xerror(enum imm_error_code err, char const *msg, char const *file,
             int line)
{
    __imm_log(IMM_LOG_ERROR, file, line, "%s: %s", __msg[err], msg);
    return (int)err;
}
