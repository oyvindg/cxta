/**
 * @file fisher.h
 * @brief Fisher Transform helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute Fisher Transform at the current view index.
 * @param[in] view Bar series view.
 * @param[in] period Lookback period.
 * @return Fisher Transform value, or 0.0 when invalid.
 */
double cxta_fisher(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
