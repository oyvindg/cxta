/**
 * @file volume_sma.h
 * @brief Volume SMA helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute volume-based SMA at the current view index.
 * @param[in] view Bar series view.
 * @param[in] period Lookback period.
 * @return Volume SMA value, or 0.0 when the view is invalid.
 */
double cxta_volume_sma(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
