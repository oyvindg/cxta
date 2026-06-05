/**
 * @file kst_math.h
 * @brief Pure KST math shared by host wrappers and CUDA fragments.
 */

#ifndef CXTA_INDICATORS_KST_MATH_H
#define CXTA_INDICATORS_KST_MATH_H

#ifndef CXTA_KST_MATH_NO_STDLIB
#include <math.h>
#endif

#ifndef CXTA_KST_MATH_FN
#define CXTA_KST_MATH_FN static inline
#endif

#ifndef CXTA_KST_MATH_FABS
#define CXTA_KST_MATH_FABS fabs
#endif

typedef struct {
    double line;
    double signal;
    double histogram;
} cxta_kst_output;

CXTA_KST_MATH_FN int cxta_kst_math_round_period(double x) {
    const int period = (int)(x >= 0.0 ? x + 0.5 : x - 0.5);
    return period < 1 ? 1 : period;
}

CXTA_KST_MATH_FN int cxta_kst_math_clamp_period(int period) {
    return period < 1 ? 1 : period;
}

CXTA_KST_MATH_FN double cxta_kst_math_roc_close(
    const double* close,
    unsigned long long index,
    int period) {
    const unsigned long long p = (unsigned long long)cxta_kst_math_clamp_period(period);
    double prev;
    if (!close || index < p) return 0.0;
    prev = close[index - p];
    if (CXTA_KST_MATH_FABS(prev) <= 1.0e-12) return 0.0;
    return 100.0 * ((close[index] - prev) / prev);
}

CXTA_KST_MATH_FN double cxta_kst_math_row_close(
    const void* rows,
    unsigned long long stride,
    unsigned long long close_offset,
    unsigned long long index) {
    const unsigned char* base = (const unsigned char*)rows;
    const double* close = (const double*)(const void*)(base + (index * stride) + close_offset);
    return *close;
}

CXTA_KST_MATH_FN double cxta_kst_math_roc_rows(
    const void* rows,
    unsigned long long stride,
    unsigned long long close_offset,
    unsigned long long index,
    int period) {
    const unsigned long long p = (unsigned long long)cxta_kst_math_clamp_period(period);
    double prev;
    if (!rows || stride == 0ull || index < p) return 0.0;
    prev = cxta_kst_math_row_close(rows, stride, close_offset, index - p);
    if (CXTA_KST_MATH_FABS(prev) <= 1.0e-12) return 0.0;
    return 100.0 *
        ((cxta_kst_math_row_close(rows, stride, close_offset, index) - prev) / prev);
}

CXTA_KST_MATH_FN double cxta_kst_math_sma_roc_close(
    const double* close,
    unsigned long long index,
    int roc_period,
    int sma_period) {
    const unsigned long long sp =
        (unsigned long long)cxta_kst_math_clamp_period(sma_period);
    const unsigned long long window = sp < (index + 1ull) ? sp : (index + 1ull);
    const unsigned long long start = index + 1ull - window;
    unsigned long long i;
    double sum = 0.0;

    if (!close || window == 0ull) return 0.0;
    for (i = start; i <= index; ++i) {
        sum += cxta_kst_math_roc_close(close, i, roc_period);
    }
    return sum / (double)window;
}

CXTA_KST_MATH_FN double cxta_kst_math_sma_roc_rows(
    const void* rows,
    unsigned long long stride,
    unsigned long long close_offset,
    unsigned long long index,
    int roc_period,
    int sma_period) {
    const unsigned long long sp =
        (unsigned long long)cxta_kst_math_clamp_period(sma_period);
    const unsigned long long window = sp < (index + 1ull) ? sp : (index + 1ull);
    const unsigned long long start = index + 1ull - window;
    unsigned long long i;
    double sum = 0.0;

    if (!rows || stride == 0ull || window == 0ull) return 0.0;
    for (i = start; i <= index; ++i) {
        sum += cxta_kst_math_roc_rows(rows, stride, close_offset, i, roc_period);
    }
    return sum / (double)window;
}

CXTA_KST_MATH_FN double cxta_kst_math_line_close(
    const double* close,
    unsigned long long index,
    int p1,
    int p2,
    int p3,
    int p4) {
    const double rcma1 = cxta_kst_math_sma_roc_close(close, index, p1, 10);
    const double rcma2 = cxta_kst_math_sma_roc_close(close, index, p2, 10);
    const double rcma3 = cxta_kst_math_sma_roc_close(close, index, p3, 10);
    const double rcma4 = cxta_kst_math_sma_roc_close(close, index, p4, 15);
    return rcma1 + 2.0 * rcma2 + 3.0 * rcma3 + 4.0 * rcma4;
}

CXTA_KST_MATH_FN double cxta_kst_math_line_rows(
    const void* rows,
    unsigned long long stride,
    unsigned long long close_offset,
    unsigned long long index,
    int p1,
    int p2,
    int p3,
    int p4) {
    const double rcma1 = cxta_kst_math_sma_roc_rows(rows, stride, close_offset, index, p1, 10);
    const double rcma2 = cxta_kst_math_sma_roc_rows(rows, stride, close_offset, index, p2, 10);
    const double rcma3 = cxta_kst_math_sma_roc_rows(rows, stride, close_offset, index, p3, 10);
    const double rcma4 = cxta_kst_math_sma_roc_rows(rows, stride, close_offset, index, p4, 15);
    return rcma1 + 2.0 * rcma2 + 3.0 * rcma3 + 4.0 * rcma4;
}

CXTA_KST_MATH_FN double cxta_kst_math_signal_close(
    const double* close,
    unsigned long long index,
    int p1,
    int p2,
    int p3,
    int p4,
    int signal_period) {
    const unsigned long long sp =
        (unsigned long long)cxta_kst_math_clamp_period(signal_period);
    const unsigned long long window = sp < (index + 1ull) ? sp : (index + 1ull);
    const unsigned long long start = index + 1ull - window;
    unsigned long long i;
    double sum = 0.0;

    if (!close || window == 0ull) return 0.0;
    for (i = start; i <= index; ++i) {
        sum += cxta_kst_math_line_close(close, i, p1, p2, p3, p4);
    }
    return sum / (double)window;
}

CXTA_KST_MATH_FN double cxta_kst_math_signal_rows(
    const void* rows,
    unsigned long long stride,
    unsigned long long close_offset,
    unsigned long long index,
    int p1,
    int p2,
    int p3,
    int p4,
    int signal_period) {
    const unsigned long long sp =
        (unsigned long long)cxta_kst_math_clamp_period(signal_period);
    const unsigned long long window = sp < (index + 1ull) ? sp : (index + 1ull);
    const unsigned long long start = index + 1ull - window;
    unsigned long long i;
    double sum = 0.0;

    if (!rows || stride == 0ull || window == 0ull) return 0.0;
    for (i = start; i <= index; ++i) {
        sum += cxta_kst_math_line_rows(rows, stride, close_offset, i, p1, p2, p3, p4);
    }
    return sum / (double)window;
}

CXTA_KST_MATH_FN cxta_kst_output cxta_kst_math_eval_close(
    const double* close,
    unsigned long long count,
    unsigned long long index,
    int p1,
    int p2,
    int p3,
    int p4,
    int signal_period) {
    cxta_kst_output out;
    out.line = 0.0;
    out.signal = 0.0;
    out.histogram = 0.0;
    if (!close || count == 0ull) return out;
    if (index >= count) index = count - 1ull;
    out.line = cxta_kst_math_line_close(close, index, p1, p2, p3, p4);
    out.signal = cxta_kst_math_signal_close(close, index, p1, p2, p3, p4, signal_period);
    out.histogram = out.line - out.signal;
    return out;
}

CXTA_KST_MATH_FN cxta_kst_output cxta_kst_math_eval_rows(
    const void* rows,
    unsigned long long count,
    unsigned long long index,
    unsigned long long stride,
    unsigned long long close_offset,
    int p1,
    int p2,
    int p3,
    int p4,
    int signal_period) {
    cxta_kst_output out;
    out.line = 0.0;
    out.signal = 0.0;
    out.histogram = 0.0;
    if (!rows || count == 0ull || stride == 0ull) return out;
    if (index >= count) index = count - 1ull;
    out.line = cxta_kst_math_line_rows(rows, stride, close_offset, index, p1, p2, p3, p4);
    out.signal =
        cxta_kst_math_signal_rows(rows, stride, close_offset, index, p1, p2, p3, p4, signal_period);
    out.histogram = out.line - out.signal;
    return out;
}

#endif /* CXTA_INDICATORS_KST_MATH_H */
