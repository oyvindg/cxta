/**
 * @file cross.c
 * @brief Cross and crossing-event primitives.
 */

#include <cxta/ts/cross.h>
#include <cxta/math/math.h>

double cxta_ts_cross(double a, double b) {
    return cxta_math_cmp(a, b);
}

double cxta_ts_crossed_above(double prev_a, double prev_b, double a, double b) {
    const double prev_cmp = cxta_math_cmp(prev_a, prev_b);
    const double curr_cmp = cxta_math_cmp(a, b);
    return (prev_cmp <= 0.0 && curr_cmp > 0.0) ? 1.0 : 0.0;
}

double cxta_ts_crossed_below(double prev_a, double prev_b, double a, double b) {
    const double prev_cmp = cxta_math_cmp(prev_a, prev_b);
    const double curr_cmp = cxta_math_cmp(a, b);
    return (prev_cmp >= 0.0 && curr_cmp < 0.0) ? 1.0 : 0.0;
}
