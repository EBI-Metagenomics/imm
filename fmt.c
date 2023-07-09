#include "fmt.h"
#include "array_size_field.h"
#include "strlcpy.h"

static struct formatter
{
  char f32[16];
  char f64[16];
} formatter = {"%.9g", "%.17g"};

void imm_fmt_set_f32(char const *fmt)
{
  imm_strlcpy(formatter.f32, fmt, imm_array_size_field(struct formatter, f32));
}

void imm_fmt_set_f64(char const *fmt)
{
  imm_strlcpy(formatter.f32, fmt, imm_array_size_field(struct formatter, f32));
}

char const *imm_fmt_get_f32(void) { return formatter.f32; }

char const *imm_fmt_get_f64(void) { return formatter.f64; }
