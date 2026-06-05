/**
 * @file wilder_step_math.h
 * @brief Header-only Wilder smoothing primitive shared by host and CUDA code.
 */

#ifndef CXTA_TS_WILDER_STEP_MATH_H
#define CXTA_TS_WILDER_STEP_MATH_H

#ifndef CXTA_WILDER_STEP_MATH_FN
#define CXTA_WILDER_STEP_MATH_FN static inline
#endif

CXTA_WILDER_STEP_MATH_FN int cxta_wilder_step_math_clamp_period(int period) {
    return period < 1 ? 1 : period;
}

CXTA_WILDER_STEP_MATH_FN double cxta_wilder_step_math_step(
    double prev,
    double x,
    int period) {
    const int p = cxta_wilder_step_math_clamp_period(period);
    return ((prev * (double)(p - 1)) + x) / (double)p;
}

#endif /* CXTA_TS_WILDER_STEP_MATH_H */
