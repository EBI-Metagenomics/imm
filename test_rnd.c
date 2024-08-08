#include "aye.h"
#include "imm_lprob.h"
#include "imm_rnd.h"
#include "near.h"

static void rnd(void)
{
  struct imm_rnd rnd = imm_rnd(1);
  aye(near(imm_rnd_dbl(&rnd), 0.01092079223));
  aye(near(imm_rnd_dbl(&rnd), 0.8859520411));
  aye((unsigned long long)imm_rnd_u64(&rnd) == 2922809869868169223ULL);
  float arr[2] = {0};
  imm_lprob_sample(&rnd, 2, arr);
  aye(near(arr[0], -0.32597934748f));
  aye(near(arr[1], -1.05687606547f));
}

int main(void)
{
  aye_begin();
  rnd();
  return aye_end();
}
