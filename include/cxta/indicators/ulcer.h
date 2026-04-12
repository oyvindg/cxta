/**
 * @file ulcer.h
 * @brief Ulcer Index helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"

/** @brief Named parameters for the Ulcer Index indicator. */
static const cxta_param_descriptor cxta_ulcer_index_params[] = {
    {"period"},
};

/** @brief Bridge-facing signature metadata for Ulcer Index. */
static const cxta_bridge_fn_spec cxta_ulcer_index_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("ulcer_index", 1u, 1u, cxta_ulcer_index_params, 1);

/** @brief Expression-facing descriptor for Ulcer Index. */
extern const cxta_indicator_descriptor cxta_ulcer_index_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute Ulcer Index at the current view index.
 * @param[in] view Bar series view.
 * @param[in] period Lookback period.
 * @return Ulcer Index, or 0.0 when invalid.
 */
double cxta_ulcer(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
