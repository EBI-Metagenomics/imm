#include "common/error.h"

static char const *msg[] = {[IMM_SUCCESS] = "success",
                            [IMM_FAILURE] = "failure",
                            [IMM_OUTOFMEM] = "out of memory",
                            [IMM_ILLEGALARG] = "illegal argument",
                            [IMM_IOERROR] = "i/o error"};

char const *explain(enum imm_error_code ecode) { return msg[ecode]; }
