#ifndef IMM_DEFER_RETURN_H
#define IMM_DEFER_RETURN_H

#define defer_return(x)                                                        \
  do                                                                           \
  {                                                                            \
    rc = x;                                                                    \
    goto defer;                                                                \
  } while (0);

#endif
