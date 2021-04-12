#ifndef UTIL_MATH_H
#define UTIL_MATH_H

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

#define DIV_ROUND_UP(n, d) (((n) + (d)-1) / (d))

unsigned long ipow(unsigned long base, unsigned exp);

#endif
