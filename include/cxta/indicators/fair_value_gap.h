/**
 * @file fair_value_gap.h
 * @brief Persistent Fair Value Gap helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"

/** @brief Named parameters for the legacy Fair Value Gap indicator. */
static const cxta_param_descriptor cxta_fair_value_gap_params[] = {
    {"lookback"},
};

/** @brief Bridge-facing metadata for the legacy Fair Value Gap indicator. */
static const cxta_bridge_fn_spec cxta_fair_value_gap_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("fair_value_gap", 0u, 1u, cxta_fair_value_gap_params, 0);

/** @brief Expression-facing descriptor for the legacy Fair Value Gap indicator. */
extern const cxta_indicator_descriptor cxta_fair_value_gap_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Fair Value Gap output payload at the current bar.
 */
typedef struct {
    double gap_high;  /**< Upper edge of the tracked gap. */
    double gap_low;   /**< Lower edge of the tracked gap. */
    double direction; /**< +1 bullish, -1 bearish, 0 none. */
    double mitigated; /**< 1.0 when the tracked gap is mitigated this bar. */
    double fill_pct;  /**< Close-based retracement into the gap, clamped to [0, 1]. */
} cxta_fair_value_gap_output;

/**
 * @brief Compute the most recent unmitigated 3-bar Fair Value Gap.
 *
 * Scans the series up to the current view index and returns the latest active
 * gap. If the active gap is touched on the current bar, the same gap is
 * returned once with `mitigated=1`.
 *
 * @param[in] view Current bar-series view.
 * @return Fair Value Gap payload for the current index.
 */
cxta_fair_value_gap_output cxta_fair_value_gap(const cxta_series_bar_view* view);

#ifdef __cplusplus
}
#endif
