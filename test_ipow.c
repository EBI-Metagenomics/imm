#include "array_size.h"
#include "ipow.h"
#include "vendor/minctest.h"
#include <stddef.h>

static void standard_usage(void)
{
  long x[] = {4, 4, 0, 1, 3, 5, 9, 50, 51};
  int e[] = {0, 1, 1, 2, 3, 4, 5, 6, 7};
  long expected[] = {1, 4, 0, 1, 27, 625, 59049, 15625000000, 897410677851};
  for (size_t i = 0; i < imm_array_size(x); ++i)
  {
    eq(imm_ipow(x[i], e[i]), expected[i]);
  }
}

int main(void)
{
  lrun("standard_usage", standard_usage);
  return lfails != 0;
}
