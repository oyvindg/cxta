/**
 * @file connors_rsi.h
 * @brief Connors RSI helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute Connors RSI at the current view index.
 * @param[in] view Bar series view.
 * @param[in] rsi_period RSI period for close.
 * @param[in] streak_period RSI period for streak.
 * @param[in] rank_period Percent-rank lookback.
 * @return Connors RSI in [0, 100], or 50.0 when invalid.
 */
double cxta_connors_rsi(const cxta_series_bar_view* view,
                        int rsi_period,
                        int streak_period,
                        int rank_period);

#ifdef __cplusplus
}
#endif
