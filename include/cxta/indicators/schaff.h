/**
 * @file schaff.h
 * @brief Schaff Trend Cycle helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute Schaff Trend Cycle at the current view index.
 * @param[in] view Bar series view.
 * @param[in] fast Fast EMA period.
 * @param[in] slow Slow EMA period.
 * @param[in] cycle Cycle period.
 * @return STC value, or 0.0 when invalid.
 */
double cxta_schaff(const cxta_series_bar_view* view, int fast, int slow, int cycle);

#ifdef __cplusplus
}
#endif
