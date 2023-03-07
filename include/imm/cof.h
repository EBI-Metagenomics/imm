#ifndef IMM_COF_H
#define IMM_COF_H

#define imm_cof(ptr, type, member)                                             \
    ({                                                                         \
        char *__mptr = (char *)(ptr);                                          \
        ((type *)(__mptr - offsetof(type, member)));                           \
    })

#define imm_cof_safe(ptr, type, member)                                        \
    ({                                                                         \
        char *__mptr = (char *)(ptr);                                          \
        __mptr == NULL ? (type *)__mptr                                        \
                       : ((type *)(__mptr - offsetof(type, member)));          \
    })

#endif
