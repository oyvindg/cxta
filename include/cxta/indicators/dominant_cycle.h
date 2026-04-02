/**
 * @file dominant_cycle.h
 * @brief Dominant cycle period helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute dominant cycle period at the current view index.
 * @param[in] view Bar series view.
 * @param[in] min_period Minimum cycle period to consider.
 * @param[in] max_period Maximum cycle period to consider.
 * @return Dominant cycle period estimate, or 0.0 when invalid.
 */
double cxta_dominant_cycle(const cxta_series_bar_view* view,
                           int min_period,
                           int max_period);

#ifdef __cplusplus
}
#endif
