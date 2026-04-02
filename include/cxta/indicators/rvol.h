/**
 * @file rvol.h
 * @brief Realized volatility helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute realized volatility at the current view index.
 *
 * Realized volatility = √(Σ r²) where r = log(close[i]/close[i-1])
 * summed over the last @p period bars.
 *
 * @param[in] view   Bar series view.
 * @param[in] period Number of log-return samples.
 * @return Realized volatility value, or 0.0 when invalid.
 */
double cxta_rvol(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
