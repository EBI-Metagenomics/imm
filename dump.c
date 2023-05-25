#include "dump.h"
#include "fmt.h"
#include <stdio.h>

void imm_dump_array_f32(size_t size, float const *array, FILE *restrict fp)
{
  fputc('[', fp);
  for (size_t i = 0; i < size; ++i)
  {
    if (i > 0) fputc(',', fp);
    fprintf(fp, IMM_FMT_F32, array[i]);
  }
  fputc(']', fp);
  fputc('\n', fp);
}

void imm_dump_array_f64(size_t size, double const *array, FILE *restrict fp)
{
  fputc('[', fp);
  for (size_t i = 0; i < size; ++i)
  {
    if (i > 0) fputc(',', fp);
    fprintf(fp, IMM_FMT_F64, array[i]);
  }
  fputc(']', fp);
  fputc('\n', fp);
}
