/**
 * @file choppiness.h
 * @brief Choppiness Index helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute Choppiness Index at the current view index.
 * @param[in] view Bar series view.
 * @param[in] period Lookback period.
 * @return Choppiness Index, or 0.0 when invalid.
 */
double cxta_choppiness(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
