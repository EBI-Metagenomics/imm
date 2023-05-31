#include "path.h"
#include "vendor/minctest.h"

static void normal_path(void)
{
  uint16_t state_ids[] = {42, 19};
  uint8_t seqlens[] = {3, 5};

  struct imm_path path = imm_path();

  imm_path_add(&path, imm_step(state_ids[0], seqlens[0]));
  imm_path_add(&path, imm_step(state_ids[1], seqlens[1]));

  eq(imm_path_step(&path, 0)->state_id, state_ids[0]);
  eq(imm_path_step(&path, 1)->state_id, state_ids[1]);

  eq(imm_path_step(&path, 0)->seqlen, seqlens[0]);
  eq(imm_path_step(&path, 1)->seqlen, seqlens[1]);

  imm_path_cleanup(&path);
}

static void long_path(void)
{
  struct imm_path path = imm_path();

  for (uint16_t i = 0; i < 1 << 14; ++i)
    imm_path_add(&path, imm_step(i, i % 16));

  for (uint16_t i = 0; i < 1 << 14; ++i)
  {
    eq(imm_path_step(&path, i)->state_id, i);
    eq(imm_path_step(&path, i)->seqlen, i % 16);
  }

  imm_path_reset(&path);

  for (uint16_t i = 0; i < 1 << 14; ++i)
    imm_path_add(&path, imm_step(i, i % 16));

  for (uint16_t i = 0; i < 1 << 14; ++i)
  {
    eq(imm_path_step(&path, i)->state_id, i);
    eq(imm_path_step(&path, i)->seqlen, i % 16);
  }

  imm_path_cleanup(&path);
}

int main(void)
{
  lrun("normal_path", normal_path);
  lrun("long_path", long_path);
  return lfails != 0;
}
