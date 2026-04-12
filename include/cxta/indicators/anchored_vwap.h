/**
 * @file anchored_vwap.h
 * @brief Anchored VWAP helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"

static const cxta_param_descriptor cxta_anchored_vwap_params[] = {
    {"anchor_bars"},
};
static const cxta_bridge_fn_spec cxta_anchored_vwap_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("anchored_vwap", 0u, 1u, cxta_anchored_vwap_params, 1);

extern const cxta_indicator_descriptor cxta_anchored_vwap_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute anchored VWAP at the current view index.
 *
 * When @p anchor_bars is `<= 0`, the anchor is the first bar of the current
 * UTC session. Otherwise the anchor is the last @p anchor_bars bars.
 *
 * @param[in] view Bar series view.
 * @param[in] anchor_bars Explicit lookback anchor, or `<= 0` for session start.
 * @return Anchored VWAP value, or 0.0 when invalid.
 */
double cxta_anchored_vwap(const cxta_series_bar_view* view, int anchor_bars);

#ifdef __cplusplus
}
#endif
