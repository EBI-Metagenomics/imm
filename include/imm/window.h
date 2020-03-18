#ifndef IMM_WINDOW_H
#define IMM_WINDOW_H

#include "imm/export.h"
#include "imm/subseq.h"
#include <stdbool.h>

struct imm_seq;
struct imm_window;

IMM_EXPORT struct imm_window* imm_window_create(struct imm_seq const* seq, unsigned length);
IMM_EXPORT struct imm_subseq  imm_window_next(struct imm_window* window);
IMM_EXPORT bool               imm_window_end(struct imm_window const* window);
IMM_EXPORT unsigned           imm_window_size(struct imm_window const* window);
IMM_EXPORT void               imm_window_destroy(struct imm_window const* window);

#endif
