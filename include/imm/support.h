#ifndef IMM_SUPPORT_H
#define IMM_SUPPORT_H

#define IMM_ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
#define IMM_STRUCT_FIELD(s, f) (((struct s *)0)->f)

#endif
