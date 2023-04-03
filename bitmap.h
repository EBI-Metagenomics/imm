#ifndef IMM_BITMAP_H
#define IMM_BITMAP_H

unsigned long imm_bitmap_get(unsigned long *, unsigned long start,
                             unsigned len);
unsigned long *imm_bitmap_reallocf(unsigned long *, unsigned long bits);
void imm_bitmap_set(unsigned long *, unsigned long val, unsigned long start,
                    unsigned len);

#endif
