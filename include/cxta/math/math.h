/**
 * @file math.h
 * @brief Core scalar math helpers for shared consumers.
 */

#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Clamp x to inclusive [lo, hi] range. */
double cxta_math_clamp(double x, double lo, double hi);
/** @brief Sign function (-1, 0, 1). */
double cxta_math_sign(double x);
/**
 * @brief Three-way comparison between two values.
 * @param[in] a Left value.
 * @param[in] b Right value.
 * @return 1.0 when a > b, -1.0 when a < b, otherwise 0.0.
 */
double cxta_math_cmp(double a, double b);
/** @brief Minimum of two values. */
double cxta_math_min2(double a, double b);
/** @brief Maximum of two values. */
double cxta_math_max2(double a, double b);
/** @brief Minimum over array; returns 0.0 for empty input. */
double cxta_math_min_n(const double* a, size_t n);
/** @brief Maximum over array; returns 0.0 for empty input. */
double cxta_math_max_n(const double* a, size_t n);

/** @brief Linear interpolation: a + (b-a)*t. */
double cxta_math_lerp(double a, double b, double t);
/** @brief Smoothstep interpolation in [0,1]. */
double cxta_math_smoothstep(double x, double e0, double e1);
/** @brief Logistic sigmoid. */
double cxta_math_sigmoid(double x, double center, double steepness);

/** @brief Constant pi. */
double cxta_math_pi(void);
/** @brief Constant e. */
double cxta_math_e(void);
/** @brief NaN value. */
double cxta_math_nan(void);
/** @brief Positive infinity. */
double cxta_math_inf(void);

/** @brief Conditional helper: cond ? a : b (cond != 0.0 means true). */
double cxta_math_if(double cond, double a, double b);

#ifdef __cplusplus
}
#endif
