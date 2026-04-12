/**
 * @file rvi.h
 * @brief Relative Vigor Index helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"

/** @brief Named parameters for the RVI indicator. */
static const cxta_param_descriptor cxta_rvi_params[] = {
    {"period"},
};

/** @brief Bridge-facing signature metadata for RVI. */
static const cxta_bridge_fn_spec cxta_rvi_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("rvi", 1u, 1u, cxta_rvi_params, 1);

/** @brief Expression-facing descriptor for RVI. */
extern const cxta_indicator_descriptor cxta_rvi_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute RVI at the current view index.
 * @param[in] view Bar series view.
 * @param[in] period Lookback period.
 * @return RVI value, or 0.0 when invalid.
 */
double cxta_rvi(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
