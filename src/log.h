#ifndef LOG_H
#define LOG_H

#include "imm/rc.h"

#define __LOG_LOCAL(n) __FILE__ ":" #n
#define __LOG_FMT(rc, msg) __LOG_LOCAL(__LINE__) ":" #rc ": " msg

enum imm_rc __log_error(enum imm_rc rc, char const *msg);

#define error(rc, msg) __log_error(rc, __LOG_FMT(rc, msg))

#endif
