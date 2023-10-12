#ifndef IMM_FMT_H
#define IMM_FMT_H

#include "compiler.h"

IMM_API void imm_fmt_set_f32(char const *);
IMM_API void imm_fmt_set_f64(char const *);

IMM_API char const *imm_fmt_get_f32(void);
IMM_API char const *imm_fmt_get_f64(void);

IMM_PURE char const *imm_f32(void) { return imm_fmt_get_f32(); }
IMM_PURE char const *imm_f64(void) { return imm_fmt_get_f64(); }

#endif
