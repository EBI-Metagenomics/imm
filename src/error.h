#ifndef ERROR_H
#define ERROR_H

#include "compiler.h"
#include "imm/error.h"
#include "imm/rc.h"

enum imm_rc __error_print(enum imm_rc rc, char const *ctx, char const *msg);
#define error(rc) __error_print(rc, LOCAL, imm_error_string(rc))

#endif
