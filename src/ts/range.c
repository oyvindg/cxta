/**
 * @file range.c
 * @brief Time-series range primitives.
 */

#include <cxta/ts/range.h>
#include <cxta/math/math.h>
#include <math.h>

double cxta_ts_true_range(double high, double low, double prev_close) {
    const double h_l = high - low;
    const double h_pc = fabs(high - prev_close);
    const double l_pc = fabs(low - prev_close);
    return cxta_math_max2(h_l, cxta_math_max2(h_pc, l_pc));
}
