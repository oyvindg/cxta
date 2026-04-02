/**
 * @file window.h
 * @brief Rolling window helpers over cxta_series_bar_view.
 */

#pragma once

#include "bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute highest `high` in inclusive range `[start, end]`.
 * @return 1 on success, 0 on invalid input/range.
 */
int cxta_series_window_highest_high(const cxta_series_bar_view* view, size_t start, size_t end, double* out);

/**
 * @brief Compute lowest `low` in inclusive range `[start, end]`.
 * @return 1 on success, 0 on invalid input/range.
 */
int cxta_series_window_lowest_low(const cxta_series_bar_view* view, size_t start, size_t end, double* out);

/**
 * @brief Midpoint of two values: `(a + b) / 2`.
 */
double cxta_series_midpoint(double a, double b);

#ifdef __cplusplus
}
#endif
