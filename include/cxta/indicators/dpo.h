/**
 * @file dpo.h
 * @brief Detrended Price Oscillator helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute DPO at the current view index.
 * @param[in] view Bar series view.
 * @param[in] period Lookback period.
 * @return DPO value, or 0.0 when invalid.
 */
double cxta_dpo(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
