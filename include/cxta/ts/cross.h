/**
 * @file cross.h
 * @brief Cross and crossing-event primitives.
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compare two scalar values.
 * @param[in] a Left value.
 * @param[in] b Right value.
 * @return 1.0 when `a > b`, -1.0 when `a < b`, otherwise 0.0.
 */
double cxta_ts_cross(double a, double b);

/**
 * @brief Detect an upward crossing between two scalar series values.
 * @param[in] prev_a Previous value of the first series.
 * @param[in] prev_b Previous value of the second series.
 * @param[in] a Current value of the first series.
 * @param[in] b Current value of the second series.
 * @return 1.0 when the relation changes from `a <= b` to `a > b`, otherwise 0.0.
 */
double cxta_ts_crossed_above(double prev_a, double prev_b, double a, double b);

/**
 * @brief Detect a downward crossing between two scalar series values.
 * @param[in] prev_a Previous value of the first series.
 * @param[in] prev_b Previous value of the second series.
 * @param[in] a Current value of the first series.
 * @param[in] b Current value of the second series.
 * @return 1.0 when the relation changes from `a >= b` to `a < b`, otherwise 0.0.
 */
double cxta_ts_crossed_below(double prev_a, double prev_b, double a, double b);

#ifdef __cplusplus
}
#endif
