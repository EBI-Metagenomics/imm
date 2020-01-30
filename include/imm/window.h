#ifndef IMM_WINDOW_H
#define IMM_WINDOW_H

#include "imm/api.h"

struct imm_seq;
struct imm_window;

IMM_API struct imm_window*    imm_window_create(struct imm_seq const* seq, unsigned length);
IMM_API struct imm_seq const* imm_window_next(struct imm_window* window);
IMM_API void                  imm_window_destroy(struct imm_window const* window);

#endif
