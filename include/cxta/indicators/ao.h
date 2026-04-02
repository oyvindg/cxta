/**
 * @file ao.h
 * @brief Awesome Oscillator helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute the Awesome Oscillator at the current view index.
 *
 * AO = SMA(5, midpoint) − SMA(34, midpoint), where midpoint = (H + L) / 2.
 *
 * @param[in] view Bar series view.
 * @return AO value, or 0.0 when the view is invalid.
 */
double cxta_ao(const cxta_series_bar_view* view);

#ifdef __cplusplus
}
#endif
