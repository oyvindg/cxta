/**
 * @file bollinger.h
 * @brief Bollinger band helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"

/** @brief Named parameters for the Bollinger indicator. */
static const cxta_param_descriptor cxta_bollinger_params[] = {
    {"period"},
    {"stddev_mult"},
};

/** @brief Bridge-facing signature metadata for Bollinger. */
static const cxta_bridge_fn_spec cxta_bollinger_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("bollinger", 2u, 2u, cxta_bollinger_params, 1);

/** @brief Expression-facing descriptor for Bollinger. */
extern const cxta_indicator_descriptor cxta_bollinger_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Bollinger output payload.
 */
typedef struct {
    double upper;
    double lower;
    double middle;
    double bandwidth;
    double percent_b;
} cxta_bollinger_output;

/**
 * @brief Compute Bollinger bands at the current view index.
 * @param[in] view Bar series view.
 * @param[in] period Lookback period.
 * @param[in] stddev_mult Standard-deviation multiplier.
 * @return Bollinger outputs.
 */
cxta_bollinger_output cxta_bollinger(const cxta_series_bar_view* view,
                                     int period,
                                     double stddev_mult);

#ifdef __cplusplus
}
#endif
