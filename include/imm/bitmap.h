#ifndef IMM_BITMAP_H
#define IMM_BITMAP_H

unsigned long imm_bitmap_get(unsigned long *x, unsigned long start,
                             unsigned len);

unsigned long *imm_bitmap_reallocf(unsigned long *x, unsigned long bits);

void imm_bitmap_set(unsigned long *x, unsigned long val, unsigned long start,
                    unsigned len);

#endif
