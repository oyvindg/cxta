/**
 * @file cmo.h
 * @brief Chande Momentum Oscillator helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute CMO at the current view index.
 * @param[in] view Bar series view.
 * @param[in] period Lookback period.
 * @return CMO value in [-100, 100], or 0.0 when invalid.
 */
double cxta_cmo(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
