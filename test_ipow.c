#include "aye.h"
#include "imm_array_size.h"
#include "imm_ipow.h"
#include <stddef.h>

static void standard_usage(void)
{
  long x[] = {4, 4, 0, 1, 3, 5, 9, 50, 51};
  int e[] = {0, 1, 1, 2, 3, 4, 5, 6, 7};
  long expected[] = {1, 4, 0, 1, 27, 625, 59049, 15625000000, 897410677851};
  for (size_t i = 0; i < imm_array_size(x); ++i)
  {
    aye(imm_ipow(x[i], e[i]) == expected[i]);
  }
}

int main(void)
{
  aye_begin();
  standard_usage();
  return aye_end();
}
