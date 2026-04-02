/**
 * @file williamsr.h
 * @brief Williams %%R helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute Williams %%R at the current view index.
 * @param[in] view Bar series view.
 * @param[in] period Lookback period.
 * @return Williams %%R in [-100, 0], or 0.0 when invalid.
 */
double cxta_williamsr(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
