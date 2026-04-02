/**
 * @file stddev.h
 * @brief Rolling standard deviation helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute population standard deviation of close prices over a rolling window.
 * @param[in] view   Bar series view.
 * @param[in] period Lookback period.
 * @return Standard deviation, or 0.0 when the view is invalid or window < 2.
 */
double cxta_stddev(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
