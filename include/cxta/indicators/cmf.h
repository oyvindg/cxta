/**
 * @file cmf.h
 * @brief Chaikin Money Flow helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute Chaikin Money Flow at the current view index.
 * @param[in] view Bar series view.
 * @param[in] period Lookback period.
 * @return CMF value, or 0.0 when invalid.
 */
double cxta_cmf(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
