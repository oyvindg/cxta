/**
 * @file hvol.h
 * @brief Historical volatility helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute annualised historical volatility at the current view index.
 *
 * Computes the population standard deviation of log returns over the last
 * @p period bars and scales by √252.
 *
 * @param[in] view   Bar series view (needs at least period+1 bars for meaningful result).
 * @param[in] period Number of log-return samples.
 * @return Annualised volatility (fraction, not percent), or 0.0 when invalid.
 */
double cxta_hvol(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
