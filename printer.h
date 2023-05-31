#ifndef IMM_PRINTER_H
#define IMM_PRINTER_H

#include "export.h"

IMM_API void imm_printer_set_f32_formatter(char const *);
IMM_API void imm_printer_set_f64_formatter(char const *);

IMM_API char const *imm_printer_get_f32_formatter(void);
IMM_API char const *imm_printer_get_f64_formatter(void);

#endif
