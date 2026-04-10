/**
 * @file wma.h
 * @brief Weighted moving average helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"

/** @brief Named parameters for the WMA indicator. */
static const cxta_param_descriptor cxta_wma_params[] = {
    {"period"},
};

/** @brief Bridge-facing signature metadata for WMA. */
static const cxta_bridge_fn_spec cxta_wma_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("wma", 1u, 1u, cxta_wma_params, 1);

/** @brief Expression-facing descriptor for WMA. */
extern const cxta_indicator_descriptor cxta_wma_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute close-based weighted moving average at the current view index.
 * @param[in] view Bar series view.
 * @param[in] period Lookback period.
 * @return Weighted moving average, or 0.0 when the view is invalid.
 */
double cxta_wma(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
