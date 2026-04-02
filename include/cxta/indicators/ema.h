/**
 * @file ema.h
 * @brief Exponential moving average helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief EMA state for incremental updates.
 */
typedef struct {
    double value;
    double initialized;
} cxta_ema_state;

/**
 * @brief Perform one EMA update.
 * @param[in] close Current close.
 * @param[in] period EMA period.
 * @param[in,out] st EMA state.
 * @return Updated EMA value.
 */
double cxta_ema_step(double close, int period, cxta_ema_state* st);

/**
 * @brief Compute close-based EMA at the current view index.
 * @param[in] view Bar series view.
 * @param[in] period EMA period.
 * @return EMA value, or 0.0 when the view is invalid.
 */
double cxta_ema(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
