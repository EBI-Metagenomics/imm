#ifndef IMM_WINDOW_H
#define IMM_WINDOW_H

#include "imm/export.h"
#include <stdbool.h>

struct imm_seq;
struct imm_window;

IMM_EXPORT struct imm_window* imm_window_create(struct imm_seq const* seq, unsigned length);
IMM_EXPORT void               imm_window_destroy(struct imm_window const* window);
IMM_EXPORT struct imm_subseq  imm_window_get(struct imm_window const* window, unsigned index);
IMM_EXPORT unsigned           imm_window_size(struct imm_window const* window);

#endif
