#include "dump.h"
#include "fmt.h"
#include "printer.h"
#include <inttypes.h>
#include <stdio.h>

void imm_dump_array_f32(size_t size, float const *array, FILE *restrict fp)
{
  fputc('[', fp);
  for (size_t i = 0; i < size; ++i)
  {
    if (i > 0) fputc(',', fp);
    fprintf(fp, imm_printer_get_f32_formatter(), array[i]);
  }
  fputc(']', fp);
}

void imm_dump_array_f64(size_t size, double const *array, FILE *restrict fp)
{
  fputc('[', fp);
  for (size_t i = 0; i < size; ++i)
  {
    if (i > 0) fputc(',', fp);
    fprintf(fp, imm_printer_get_f64_formatter(), array[i]);
  }
  fputc(']', fp);
}

void imm_dump_array_u8(size_t size, uint8_t const *array, FILE *restrict fp)
{
  fputc('[', fp);
  for (size_t i = 0; i < size; ++i)
  {
    if (i > 0) fputc(',', fp);
    fprintf(fp, "%" PRIu8, array[i]);
  }
  fputc(']', fp);
}

void imm_dump_array_u16(size_t size, uint16_t const *array, FILE *restrict fp)
{
  fputc('[', fp);
  for (size_t i = 0; i < size; ++i)
  {
    if (i > 0) fputc(',', fp);
    fprintf(fp, "%" PRIu16, array[i]);
  }
  fputc(']', fp);
}

void imm_dump_array_u32(size_t size, uint32_t const *array, FILE *restrict fp)
{
  fputc('[', fp);
  for (size_t i = 0; i < size; ++i)
  {
    if (i > 0) fputc(',', fp);
    fprintf(fp, "%" PRIu32, array[i]);
  }
  fputc(']', fp);
}
