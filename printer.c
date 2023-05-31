#include "array_size_field.h"
#include "fmt.h"
#include "strlcpy.h"

static struct formatter
{
  char f32[16];
  char f64[16];
} formatter = {IMM_FMT_F32, IMM_FMT_F64};

void imm_printer_set_f32_formatter(char const *fmt)
{
  imm_strlcpy(formatter.f32, fmt, imm_array_size_field(struct formatter, f32));
}

void imm_printer_set_f64_formatter(char const *fmt)
{
  imm_strlcpy(formatter.f32, fmt, imm_array_size_field(struct formatter, f32));
}

char const *imm_printer_get_f32_formatter(void) { return formatter.f32; }

char const *imm_printer_get_f64_formatter(void) { return formatter.f64; }
