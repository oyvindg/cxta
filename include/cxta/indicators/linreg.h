/**
 * @file linreg.h
 * @brief Linear regression slope and angle helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"

static const cxta_param_descriptor cxta_linear_regression_slope_params[] = {
    {"period"},
};
static const cxta_bridge_fn_spec cxta_linear_regression_slope_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("linear_regression_slope", 1u, 1u, cxta_linear_regression_slope_params, 1);

static const cxta_param_descriptor cxta_linreg_angle_params[] = {
    {"period"},
};
static const cxta_bridge_fn_spec cxta_linreg_angle_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("linreg_angle", 1u, 1u, cxta_linreg_angle_params, 1);

extern const cxta_indicator_descriptor cxta_linear_regression_slope_descriptor;
extern const cxta_indicator_descriptor cxta_linreg_angle_descriptor;

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
