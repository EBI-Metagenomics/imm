#ifndef BITMAP_H
#define BITMAP_H

unsigned long *bitmap_alloc(unsigned long bits);

unsigned long bitmap_get(unsigned long *x, unsigned long start, unsigned len);

unsigned long *bitmap_realloc(unsigned long *x, unsigned long bits);

void bitmap_set(unsigned long *x, unsigned long val, unsigned long start,
                unsigned len);

#endif
