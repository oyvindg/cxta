/**
 * @file bollinger_math.h
 * @brief Pure Bollinger math shared by host wrappers and CUDA fragments.
 */

#ifndef CXTA_INDICATORS_BOLLINGER_MATH_H
#define CXTA_INDICATORS_BOLLINGER_MATH_H

#ifndef CXTA_BOLLINGER_MATH_NO_STDLIB
#include <math.h>
#endif

#ifndef CXTA_BOLLINGER_MATH_FN
#define CXTA_BOLLINGER_MATH_FN static inline
#endif

#ifndef CXTA_BOLLINGER_MATH_SQRT
#define CXTA_BOLLINGER_MATH_SQRT sqrt
#endif

#ifndef CXTA_BOLLINGER_MATH_FABS
#define CXTA_BOLLINGER_MATH_FABS fabs
#endif

typedef struct {
    double upper;
    double lower;
    double middle;
    double bandwidth;
    double percent_b;
} cxta_bollinger_output;

CXTA_BOLLINGER_MATH_FN int cxta_bollinger_math_round_period(double x) {
    const int period = (int)(x >= 0.0 ? x + 0.5 : x - 0.5);
    return period < 1 ? 1 : period;
}

CXTA_BOLLINGER_MATH_FN int cxta_bollinger_math_clamp_period(int period) {
    return period < 1 ? 1 : period;
}

CXTA_BOLLINGER_MATH_FN double cxta_bollinger_math_row_close(
    const void* rows,
    unsigned long long stride,
    unsigned long long close_offset,
    unsigned long long index) {
    const unsigned char* base = (const unsigned char*)rows;
    const double* close = (const double*)(const void*)(base + (index * stride) + close_offset);
    return *close;
}

CXTA_BOLLINGER_MATH_FN cxta_bollinger_output cxta_bollinger_math_eval_rows(
    const void* rows,
    unsigned long long count,
    unsigned long long index,
    unsigned long long stride,
    unsigned long long close_offset,
    int period,
    double stddev_mult) {
    cxta_bollinger_output out;
    const unsigned long long p = (unsigned long long)cxta_bollinger_math_clamp_period(period);
    unsigned long long window;
    unsigned long long start;
    unsigned long long i;
    double sum = 0.0;
    double sum_sq = 0.0;
    double mean;
    double variance;
    double sd;
    double width;

    out.upper = 0.0;
    out.lower = 0.0;
    out.middle = 0.0;
    out.bandwidth = 0.0;
    out.percent_b = 0.5;
    if (!rows || count == 0ull || stride == 0ull) return out;

    if (index >= count) index = count - 1ull;
    window = p < (index + 1ull) ? p : (index + 1ull);
    start = index + 1ull - window;

    for (i = start; i <= index; ++i) {
        const double close = cxta_bollinger_math_row_close(rows, stride, close_offset, i);
        sum += close;
        sum_sq += close * close;
    }

    mean = sum / (double)window;
    variance = (sum_sq / (double)window) - (mean * mean);
    if (variance < 0.0) variance = 0.0;
    sd = CXTA_BOLLINGER_MATH_SQRT(variance);

    out.middle = mean;
    out.upper = mean + (stddev_mult * sd);
    out.lower = mean - (stddev_mult * sd);
    out.bandwidth = CXTA_BOLLINGER_MATH_FABS(out.middle) > 1.0e-12
        ? (out.upper - out.lower) / out.middle
        : 0.0;
    width = out.upper - out.lower;
    out.percent_b = CXTA_BOLLINGER_MATH_FABS(width) > 1.0e-12
        ? (cxta_bollinger_math_row_close(rows, stride, close_offset, index) - out.lower) / width
        : 0.5;
    return out;
}

CXTA_BOLLINGER_MATH_FN cxta_bollinger_output cxta_bollinger_math_eval_close(
    const double* close,
    unsigned long long count,
    unsigned long long index,
    int period,
    double stddev_mult) {
    return cxta_bollinger_math_eval_rows(
        close,
        count,
        index,
        (unsigned long long)sizeof(close[0]),
        0ull,
        period,
        stddev_mult);
}

#endif /* CXTA_INDICATORS_BOLLINGER_MATH_H */
