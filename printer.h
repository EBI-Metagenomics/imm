#ifndef IMM_PRINTER_H
#define IMM_PRINTER_H

#include "export.h"

IMM_API void imm_printer_set_f32fmt(char const *);
IMM_API void imm_printer_set_f64fmt(char const *);

IMM_API char const *imm_printer_get_f32fmt(void);
IMM_API char const *imm_printer_get_f64fmt(void);

#endif
