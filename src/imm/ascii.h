#ifndef IMM_ASCII_H
#define IMM_ASCII_H

#include "src/imm/hide.h"
#include <stddef.h>

#define LAST_STD_ASCII 127

HIDE int is_std_ascii(char const *str, size_t len);

#endif
