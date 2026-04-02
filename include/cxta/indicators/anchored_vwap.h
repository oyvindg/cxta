/**
 * @file anchored_vwap.h
 * @brief Anchored VWAP helpers.
 */

#pragma once

#include "../series/bar.h"

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
