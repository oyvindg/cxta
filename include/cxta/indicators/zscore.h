/**
 * @file zscore.h
 * @brief Rolling Z-score helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"

/** @brief Named parameters for the Z-score indicator. */
static const cxta_param_descriptor cxta_zscore_params[] = {
    {"period"},
};

/** @brief Bridge-facing signature metadata for Z-score. */
static const cxta_bridge_fn_spec cxta_zscore_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("zscore", 1u, 1u, cxta_zscore_params, 1);

/** @brief Expression-facing descriptor for Z-score. */
extern const cxta_indicator_descriptor cxta_zscore_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute rolling Z-score of the close at the current view index.
 *
 * Z = (close - rolling_mean) / rolling_stddev over @p period bars.
 *
 * @param[in] view   Bar series view.
 * @param[in] period Lookback period.
 * @return Z-score, or 0.0 when the view is invalid or stddev is zero.
 */
double cxta_zscore(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
