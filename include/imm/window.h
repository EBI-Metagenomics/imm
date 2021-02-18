#ifndef IMM_WINDOW_H
#define IMM_WINDOW_H

#include "imm/export.h"
#include <stdint.h>

struct imm_seq;
struct imm_window;

#define IMM_WINDOW_MAX_LEN UINT16_MAX

IMM_API struct imm_window* imm_window_create(struct imm_seq const* seq, uint16_t length);
IMM_API void               imm_window_destroy(struct imm_window const* window);
IMM_API struct imm_subseq  imm_window_get(struct imm_window const* window, uint16_t index);
IMM_API uint16_t           imm_window_size(struct imm_window const* window);

#endif
