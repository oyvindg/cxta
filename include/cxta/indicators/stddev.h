/**
 * @file stddev.h
 * @brief Rolling standard deviation helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"

/** @brief Named parameters for the StdDev indicator. */
static const cxta_param_descriptor cxta_stddev_params[] = {
    {"period"},
};

/** @brief Bridge-facing signature metadata for StdDev. */
static const cxta_bridge_fn_spec cxta_stddev_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("stddev", 1u, 1u, cxta_stddev_params, 1);

/** @brief Expression-facing descriptor for StdDev. */
extern const cxta_indicator_descriptor cxta_stddev_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute population standard deviation of close prices over a rolling window.
 * @param[in] view   Bar series view.
 * @param[in] period Lookback period.
 * @return Standard deviation, or 0.0 when the view is invalid or window < 2.
 */
double cxta_stddev(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
