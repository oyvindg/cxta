/**
 * @file range.h
 * @brief Time-series range primitives.
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute the true range for a bar.
 * @param[in] high Current high.
 * @param[in] low Current low.
 * @param[in] prev_close Previous close.
 * @return Maximum of `high - low`, `abs(high - prev_close)`, and `abs(low - prev_close)`.
 */
double cxta_ts_true_range(double high, double low, double prev_close);

#ifdef __cplusplus
}
#endif
