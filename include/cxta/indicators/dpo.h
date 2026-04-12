/**
 * @file dpo.h
 * @brief Detrended Price Oscillator helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"

/** @brief Named parameters for the DPO indicator. */
static const cxta_param_descriptor cxta_dpo_params[] = {
    {"period"},
};

/** @brief Bridge-facing signature metadata for DPO. */
static const cxta_bridge_fn_spec cxta_dpo_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("dpo", 1u, 1u, cxta_dpo_params, 1);

/** @brief Expression-facing descriptor for DPO. */
extern const cxta_indicator_descriptor cxta_dpo_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute DPO at the current view index.
 * @param[in] view Bar series view.
 * @param[in] period Lookback period.
 * @return DPO value, or 0.0 when invalid.
 */
double cxta_dpo(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
