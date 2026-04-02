/**
 * @file cci.h
 * @brief Commodity Channel Index helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute CCI at the current view index.
 * @param[in] view Bar series view.
 * @param[in] period Lookback period.
 * @return CCI value, or 0.0 when invalid.
 */
double cxta_cci(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
