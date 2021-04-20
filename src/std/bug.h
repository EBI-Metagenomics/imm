#ifndef STD_BUG_H
#define STD_BUG_H

void std_bug_print_exit(char const* file, char const* func, int line, char const* cond);

#define BUG(cond)                                                                                                      \
    do {                                                                                                               \
        if (!(bool)(cond))                                                                                             \
            break;                                                                                                     \
        std_bug_print_exit(__FILE__, __func__, __LINE__, #cond);                                                       \
    } while (0)

#endif
