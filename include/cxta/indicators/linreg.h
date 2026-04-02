/**
 * @file linreg.h
 * @brief Linear regression slope and angle helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute least-squares slope of close prices over a rolling window.
 * @param[in] view   Bar series view.
 * @param[in] period Window length.
 * @return Slope value, or 0.0 when the view is invalid or window < 2.
 */
double cxta_linreg_slope(const cxta_series_bar_view* view, int period);

/**
 * @brief Compute linear regression angle (atan of slope) in degrees.
 * @param[in] view   Bar series view.
 * @param[in] period Window length.
 * @return Angle in degrees.
 */
double cxta_linreg_angle(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
