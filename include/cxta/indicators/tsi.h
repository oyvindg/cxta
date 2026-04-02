/**
 * @file tsi.h
 * @brief True Strength Index helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief TSI state for incremental updates.
 */
typedef struct {
    double ema_m1;       /**< First EMA of momentum. */
    double ema_abs_m1;   /**< First EMA of absolute momentum. */
    double ema_m2;       /**< Second EMA of momentum. */
    double ema_abs_m2;   /**< Second EMA of absolute momentum. */
    double prev_close;   /**< Previous close. */
    double initialized;  /**< Non-zero once seeded. */
} cxta_tsi_state;

/**
 * @brief Perform one TSI update.
 * @param[in] close Current close.
 * @param[in] long_period First EMA period.
 * @param[in] short_period Second EMA period.
 * @param[in,out] st TSI state.
 * @return Updated TSI value.
 */
double cxta_tsi_step(double close, int long_period, int short_period, cxta_tsi_state* st);

/**
 * @brief Compute TSI at the current view index.
 * @param[in] view Bar series view.
 * @param[in] long_period First EMA period.
 * @param[in] short_period Second EMA period.
 * @return TSI value, or 0.0 when invalid.
 */
double cxta_tsi(const cxta_series_bar_view* view, int long_period, int short_period);

#ifdef __cplusplus
}
#endif
