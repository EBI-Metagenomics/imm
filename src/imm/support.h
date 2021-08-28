#ifndef IMM_SUPPORT_H
#define IMM_SUPPORT_H

#ifdef __GNUC__
#define imm_likely(x) __builtin_expect(!!(x), 1)
#define imm_unlikely(x) __builtin_expect(!!(x), 0)
#else
#define imm_likely(x) (x)
#define imm_unlikely(x) (x)
#endif

#define IMM_ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

#ifdef NDEBUG
#define IMM_BUG(cond)
#else
#define IMM_BUG(cond)                                                          \
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
