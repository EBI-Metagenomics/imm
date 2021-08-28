#ifndef IMM_BUG_H
#define IMM_BUG_H

#ifdef NDEBUG
#define __IMM_BUG(cond)
#else
#define __IMM_BUG(cond)                                                        \
    do                                                                         \
    {                                                                          \
        if (!(cond))                                                           \
            break;                                                             \
        __imm_bug(__FILE__, __LINE__, #cond);                                  \
    } while (0)
#endif

void __imm_bug(char const *file, int line, char const *cond)
    __attribute__((noreturn));

#endif
