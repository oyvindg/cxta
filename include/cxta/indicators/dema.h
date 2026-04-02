/**
 * @file dema.h
 * @brief Double Exponential Moving Average helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief DEMA state for incremental updates.
 */
typedef struct {
    double ema1;        /**< First EMA value. */
    double ema2;        /**< EMA of EMA value. */
    double initialized; /**< Non-zero once seeded. */
} cxta_dema_state;

/**
 * @brief Perform one DEMA update.
 * @param[in]     close  Current close.
 * @param[in]     period EMA period.
 * @param[in,out] st     DEMA state.
 * @return DEMA value (2*EMA - EMA(EMA)).
 */
double cxta_dema_step(double close, int period, cxta_dema_state* st);

/**
 * @brief Compute close-based DEMA at the current view index.
 * @param[in] view   Bar series view.
 * @param[in] period EMA period.
 * @return DEMA value, or 0.0 when the view is invalid.
 */
double cxta_dema(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
