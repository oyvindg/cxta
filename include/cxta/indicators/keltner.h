/**
 * @file keltner.h
 * @brief Keltner channel helpers.
 */

#pragma once

#include "donchian.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute a Keltner channel at the current view index.
 * @param[in] view Bar series view.
 * @param[in] ema_period EMA period for the middle line.
 * @param[in] atr_mult ATR multiplier.
 * @param[in] atr_period ATR period.
 * @return Channel outputs.
 */
cxta_channel_output cxta_keltner(const cxta_series_bar_view* view,
                                 int ema_period,
                                 double atr_mult,
                                 int atr_period);

#ifdef __cplusplus
}
#endif
