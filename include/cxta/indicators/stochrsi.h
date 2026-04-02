/**
 * @file stochrsi.h
 * @brief Stoch RSI helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Stoch RSI output payload.
 */
typedef struct {
    double k; /**< Smoothed %K. */
    double d; /**< Smoothed %D. */
} cxta_stochrsi_output;

/**
 * @brief Compute Stoch RSI outputs at the current view index.
 * @param[in] view Bar series view.
 * @param[in] rsi_period RSI period.
 * @param[in] stoch_period Stochastic lookback over RSI.
 * @param[in] smooth_k %K smoothing period.
 * @param[in] smooth_d %D smoothing period.
 * @return Stoch RSI outputs, or a zero-initialized struct when invalid.
 */
cxta_stochrsi_output cxta_stochrsi(const cxta_series_bar_view* view,
                                   int rsi_period,
                                   int stoch_period,
                                   int smooth_k,
                                   int smooth_d);

#ifdef __cplusplus
}
#endif
