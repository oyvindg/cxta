/**
 * @file williamsr.h
 * @brief Williams %%R helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"

/** @brief Named parameters for Williams %%R. */
static const cxta_param_descriptor cxta_williams_r_params[] = {
    {"period"},
};

/** @brief Bridge-facing signature metadata for Williams %R. */
static const cxta_bridge_fn_spec cxta_williams_r_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("williams_r", 1u, 1u, cxta_williams_r_params, 1);

/** @brief Expression-facing descriptor for Williams %R. */
extern const cxta_indicator_descriptor cxta_williams_r_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute Williams %%R at the current view index.
 * @param[in] view Bar series view.
 * @param[in] period Lookback period.
 * @return Williams %%R in [-100, 0], or 0.0 when invalid.
 */
double cxta_williamsr(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
