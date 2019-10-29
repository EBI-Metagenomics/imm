#ifndef IMM_ASCII_H
#define IMM_ASCII_H

#include "src/imm/hide.h"
#include <stddef.h>

#define ASCII_LAST_STD 127

HIDE int ascii_is_std(char const* str, size_t len);

#endif
