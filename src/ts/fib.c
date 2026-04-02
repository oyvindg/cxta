/**
 * @file fib.c
 * @brief Fibonacci retracement and extension scalar predicates.
 */

#include <cxta/ts/fib.h>
#include <cxta/math/math.h>
#include <math.h>

double cxta_ts_fib_level(double high, double low, double ratio) {
    if (high <= low) return 0.0;
    return cxta_math_lerp(high, low, ratio);
}

double cxta_ts_fib_near(double high, double low, double close,
                     double ratio, double tolerance) {
    if (high <= low) return 0.0;
    const double range = high - low;
    const double level = cxta_math_lerp(high, low, ratio);
    return (fabs(close - level) <= range * tolerance) ? 1.0 : 0.0;
}

double cxta_ts_fib_zone(double high, double low, double close,
                     double ratio_lo, double ratio_hi) {
    if (high <= low) return 0.0;
    if (ratio_lo >= ratio_hi) return 0.0;
    const double upper = cxta_math_lerp(high, low, ratio_lo);
    const double lower = cxta_math_lerp(high, low, ratio_hi);
    return (close >= lower && close <= upper) ? 1.0 : 0.0;
}

double cxta_ts_fib_extension(double swing_low, double swing_high,
                          double retrace_low, double ratio) {
    if (swing_high <= swing_low) return 0.0;
    return retrace_low + (swing_high - swing_low) * ratio;
}
