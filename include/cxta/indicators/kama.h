/**
 * @file kama.h
 * @brief Kaufman Adaptive Moving Average helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute KAMA at the current view index.
 * @param[in] view Bar series view.
 * @param[in] period Efficiency-ratio lookback period.
 * @return KAMA value, or 0.0 when invalid.
 */
double cxta_kama(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
