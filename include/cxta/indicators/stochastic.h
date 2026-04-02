/**
 * @file stochastic.h
 * @brief Stochastic oscillator helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Stochastic output payload.
 */
typedef struct {
    double k; /**< Smoothed %K. */
    double d; /**< Smoothed %D. */
} cxta_stoch_output;

/**
 * @brief Compute stochastic outputs at the current view index.
 * @param[in] view Bar series view.
 * @param[in] k_period Lookback for the raw %K.
 * @param[in] d_period Smoothing period for %D.
 * @param[in] smooth_k Smoothing period for %K.
 * @return Stochastic outputs, or a zero-initialized struct when invalid.
 */
cxta_stoch_output cxta_stochastic(const cxta_series_bar_view* view,
                                  int k_period,
                                  int d_period,
                                  int smooth_k);

#ifdef __cplusplus
}
#endif
