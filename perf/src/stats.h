#ifndef PERF_STATS_H
#define PERF_STATS_H

#include <math.h>

static inline double __mean(double const* arr, unsigned n);
static inline double __sample_std(double const* arr, unsigned n, double mean);

struct stats
{
    double mean; /**< Arithmetic mean. */
    double sem;  /**< Standard error of the mean. */
};

static inline struct stats compute_stats(double const* arr, unsigned n)
{
    double mean = __mean(arr, n);
    double std = __sample_std(arr, n, mean);
    double sem = std / sqrt(n);
    return (struct stats){mean, sem};
}

static inline double __mean(double const* arr, unsigned n)
{
    if (n == 0)
        return NAN;

    double total = arr[0];

    for (unsigned i = 1; i < n; ++i)
        total += arr[i];

    return total / n;
}

static inline double __sample_std(double const* arr, unsigned n, double mean)
{
    if (n <= 1)
        return NAN;

    double var = 0.0;

    for (unsigned i = 0; i < n; ++i)
        var += (arr[i] - mean) * (arr[i] - mean);

    return sqrt(var / (n - 1));
}

#endif
