#ifndef SWAP_H
#define SWAP_H

#define swap(a, b)                                                             \
    do                                                                         \
    {                                                                          \
        __typeof__(a) t = (a);                                                 \
        (a) = (b);                                                             \
        (b) = t;                                                               \
    } while (0);

#endif
