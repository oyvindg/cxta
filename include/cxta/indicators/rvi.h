/**
 * @file rvi.h
 * @brief Relative Vigor Index helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute RVI at the current view index.
 * @param[in] view Bar series view.
 * @param[in] period Lookback period.
 * @return RVI value, or 0.0 when invalid.
 */
double cxta_rvi(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
