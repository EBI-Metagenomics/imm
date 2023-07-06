#ifndef IMM_COF_H
#define IMM_COF_H

#define imm_container(ptr, type, member)                                       \
  ({                                                                           \
    char *__mptr = (char *)(ptr);                                              \
    ((type *)(__mptr - offsetof(type, member)));                               \
  })

#endif
