/**
 * @file ulcer.h
 * @brief Ulcer Index helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute Ulcer Index at the current view index.
 * @param[in] view Bar series view.
 * @param[in] period Lookback period.
 * @return Ulcer Index, or 0.0 when invalid.
 */
double cxta_ulcer(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
