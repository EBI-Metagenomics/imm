#ifndef ERROR_H
#define ERROR_H

#include "compiler.h"
#include "imm/error.h"
#include "imm/rc.h"

int __imm_error_print(int rc, char const *ctx, char const *msg);
#define error(rc) __imm_error_print(rc, LOCAL, imm_error_string(rc))

#endif
