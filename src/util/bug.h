#ifndef UTIL_BUG_H
#define UTIL_BUG_H

void util_bug_print_exit(char const* file, char const* func, int line, char const* cond);

#define BUG(cond)                                                                                                      \
    do {                                                                                                               \
        if (!(cond))                                                                                                   \
            break;                                                                                                     \
        util_bug_print_exit(__FILE__, __func__, __LINE__, #cond);                                                      \
    } while (0)

#endif
