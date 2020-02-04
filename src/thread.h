#ifndef IMM_THREAD_H
#define IMM_THREAD_H

#ifdef OPENMP
#include <omp.h>
static inline unsigned get_num_threads(void) { return (unsigned)omp_get_num_threads(); }
static inline unsigned get_thread_num(void) { return (unsigned)omp_get_thread_num(); }
#else
static inline unsigned get_num_threads(void) { return 1; }
static inline unsigned get_thread_num(void) { return 0; }
#endif

#endif
