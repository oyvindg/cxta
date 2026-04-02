/**
 * @file mass_index.h
 * @brief Mass Index helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute Mass Index at the current view index.
 * @param[in] view Bar series view.
 * @param[in] sum_period Rolling ratio-sum period.
 * @param[in] ema_period EMA period for the range smoothing.
 * @return Mass Index value, or 0.0 when invalid.
 */
double cxta_mass_index(const cxta_series_bar_view* view, int sum_period, int ema_period);

#ifdef __cplusplus
}
#endif
