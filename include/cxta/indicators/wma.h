/**
 * @file wma.h
 * @brief Weighted moving average helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute close-based weighted moving average at the current view index.
 * @param[in] view Bar series view.
 * @param[in] period Lookback period.
 * @return Weighted moving average, or 0.0 when the view is invalid.
 */
double cxta_wma(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
