#ifndef ERROR_H
#define ERROR_H

#include "imm/rc.h"

#define __ERROR_STRINGIFY(n) #n
#define __ERROR_LOCAL(n) __FILE__ ":" __ERROR_STRINGIFY(n)
#define __ERROR_FMT(rc, msg) __ERROR_LOCAL(__LINE__) ":" #rc ": " msg

enum imm_rc __error(enum imm_rc rc, char const *msg);

#define error(rc, msg) __error(rc, __ERROR_FMT(rc, msg))

#endif
