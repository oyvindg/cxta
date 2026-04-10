/**
 * @file sma.h
 * @brief Simple moving average helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"

#include <stddef.h>

/** @brief Named parameters for the SMA indicator. */
static const cxta_param_descriptor cxta_sma_params[] = {
    {"period"},
};

/** @brief Bridge-facing signature metadata for SMA. */
static const cxta_bridge_fn_spec cxta_sma_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("sma", 1u, 1u, cxta_sma_params, 1);

/** @brief Expression-facing descriptor for SMA. */
extern const cxta_indicator_descriptor cxta_sma_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute SMA from rolling sum and window length.
 * @param[in] sum Window sum.
 * @param[in] window Window length.
 * @return Average value, or 0.0 when @p window is zero.
 */
double cxta_sma_from_sum(double sum, size_t window);

/**
 * @brief Compute close-based SMA at the current view index.
 * @param[in] view Bar series view.
 * @param[in] period Lookback period.
 * @return SMA value, or 0.0 when the view is invalid.
 */
double cxta_sma(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
