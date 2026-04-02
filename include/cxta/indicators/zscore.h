/**
 * @file zscore.h
 * @brief Rolling Z-score helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute rolling Z-score of the close at the current view index.
 *
 * Z = (close - rolling_mean) / rolling_stddev over @p period bars.
 *
 * @param[in] view   Bar series view.
 * @param[in] period Lookback period.
 * @return Z-score, or 0.0 when the view is invalid or stddev is zero.
 */
double cxta_zscore(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
