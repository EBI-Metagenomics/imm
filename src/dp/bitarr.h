#ifndef DP_BITARR_H
#define DP_BITARR_H

unsigned long *bitarr_alloc(unsigned long bits);

unsigned long bitarr_get(unsigned long *x, unsigned long start, unsigned len);

unsigned long *bitarr_realloc(unsigned long *x, unsigned long bits);

void bitarr_set(unsigned long *x, unsigned long val, unsigned long start,
                unsigned len);

#endif
