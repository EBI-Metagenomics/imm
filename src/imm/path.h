#ifndef IMM_PATH_H
#define IMM_PATH_H

#include "src/imm/hide.h"

struct imm_path;
struct imm_state;

HIDE const struct imm_path *path_next_item(const struct imm_path *item);
HIDE const struct imm_state *path_item_state(const struct imm_path *item);
HIDE int path_item_seq_len(const struct imm_path *item);
HIDE int path_seq_len(const struct imm_path *path);

#endif
