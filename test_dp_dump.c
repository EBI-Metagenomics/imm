#include "imm/imm.h"
#include "vendor/minctest.h"

static void dp_dump_ex1(void)
{
  imm_ex1_init(10);
  struct imm_ex1 *m = &imm_ex1;
  struct imm_dp dp;
  eq(imm_hmm_init_dp(&imm_ex1.hmm, &m->end.super, &dp), 0);

  FILE *fp = fopen("dp_ex1.dump", "wb");
  imm_dp_dump(&dp, &imm_ex1_state_name, fp);
  fclose(fp);

  imm_dp_del(&dp);
  remove("dp_ex1.dump");
}

int main(void)
{
  lrun("dp_dump_ex1", dp_dump_ex1);
  return lfails != 0;
}
