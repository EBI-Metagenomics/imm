#ifndef IMM_THREAD_H
#define IMM_THREAD_H

#ifdef OPENMP
#include <omp.h>

static inline unsigned thread_max_size(void) { return (unsigned)omp_get_max_threads(); }
static inline unsigned thread_size(void) { return (unsigned)omp_get_num_threads(); }
static inline unsigned thread_id(void) { return (unsigned)omp_get_thread_num(); }
#else
static inline unsigned thread_max_size(void) { return 1; }
static inline unsigned thread_size(void) { return 1; }
static inline unsigned thread_id(void) { return 0; }
#endif

#endif
