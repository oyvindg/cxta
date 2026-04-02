/**
 * @file hma.h
 * @brief Hull Moving Average helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute close-based Hull Moving Average at the current view index.
 *
 * HMA = WMA(2·WMA(n/2) − WMA(n), √n)
 *
 * @param[in] view   Bar series view.
 * @param[in] period Lookback period (>= 2).
 * @return HMA value, or 0.0 when the view is invalid.
 */
double cxta_hma(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
