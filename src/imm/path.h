#ifndef IMM_PATH_H
#define IMM_PATH_H

#include "src/imm/hide.h"

struct imm_path;
struct imm_state;
struct imm_step;

HIDE int  path_seq_len(const struct imm_path* path);
HIDE void path_add_head(struct imm_path* path, struct imm_state const* state, int seq_len);

#endif
