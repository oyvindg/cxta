/**
 * @file sma.h
 * @brief Simple moving average helpers.
 */

#pragma once

#include "../series/bar.h"

#include <stddef.h>

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
