/**
 * @file kama.h
 * @brief Kaufman Adaptive Moving Average helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"

/** @brief Named parameters for the KAMA indicator. */
static const cxta_param_descriptor cxta_kama_params[] = {
    {"period"},
};

/** @brief Bridge-facing signature metadata for KAMA. */
static const cxta_bridge_fn_spec cxta_kama_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("kama", 1u, 1u, cxta_kama_params, 1);

/** @brief Expression-facing descriptor for KAMA. */
extern const cxta_indicator_descriptor cxta_kama_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute KAMA at the current view index.
 * @param[in] view Bar series view.
 * @param[in] period Efficiency-ratio lookback period.
 * @return KAMA value, or 0.0 when invalid.
 */
double cxta_kama(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
