/**
 * @file zigzag.h
 * @brief Causal ZigZag helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"

/** @brief Named parameters for the ZigZag indicator. */
static const cxta_param_descriptor cxta_zigzag_params[] = {
    {"threshold"},
    {"pivot_offset"},
};

/** @brief Bridge-facing cxpr signature metadata for ZigZag. */
static const cxta_bridge_fn_spec cxta_zigzag_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("zigzag", 1u, 2u, cxta_zigzag_params, 1);

/** @brief Expression-facing descriptor for ZigZag. */
extern const cxta_indicator_descriptor cxta_zigzag_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ZigZag output payload.
 */
typedef struct {
    double high;
    double low;
    double line;
    double pivot_index;
    double active;
    double active_index;
    double direction;
    double last;
    double is_high;
} cxta_zigzag_output;

/**
 * @brief Compute causal ZigZag outputs at the current view index.
 * @param[in] view Bar series view.
 * @param[in] threshold Reversal threshold as a fraction (e.g. `0.03` = 3%).
 * @param[in] pivot_offset History index (`0` = newest pivot).
 * @return ZigZag outputs, or a zero-initialized struct when invalid.
 */
cxta_zigzag_output cxta_zigzag(const cxta_series_bar_view* view,
                               double threshold,
                               int pivot_offset);

#ifdef __cplusplus
}
#endif
