#include "lprob.h"
#include "rnd.h"
#include "vendor/minctest.h"

static void rnd(void)
{
  struct imm_rnd rnd = imm_rnd(1);
  close(imm_rnd_dbl(&rnd), 0.01092079223);
  close(imm_rnd_dbl(&rnd), 0.8859520411);
  eq((unsigned long long)imm_rnd_u64(&rnd), 2922809869868169223ULL);
  float arr[2] = {0};
  imm_lprob_sample(&rnd, 2, arr);
  close(arr[0], -0.32597934748);
  close(arr[1], -1.05687606547);
}

int main()
{
  lrun("rnd", rnd);
  return lfails != 0;
}
