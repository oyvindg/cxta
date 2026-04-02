/**
 * @file chaikin.h
 * @brief Chaikin Oscillator helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Chaikin Oscillator state for incremental updates.
 */
typedef struct {
    double adl;         /**< Accumulation/distribution line. */
    double ema_fast;    /**< Fast EMA of ADL. */
    double ema_slow;    /**< Slow EMA of ADL. */
    double initialized; /**< Non-zero once seeded. */
} cxta_chaikin_state;

/**
 * @brief Perform one Chaikin Oscillator update.
 * @param[in] high Current high.
 * @param[in] low Current low.
 * @param[in] close Current close.
 * @param[in] volume Current volume.
 * @param[in] fast Fast EMA period.
 * @param[in] slow Slow EMA period.
 * @param[in,out] st Chaikin state.
 * @return Updated Chaikin Oscillator value.
 */
double cxta_chaikin_step(double high,
                         double low,
                         double close,
                         double volume,
                         int fast,
                         int slow,
                         cxta_chaikin_state* st);

/**
 * @brief Compute Chaikin Oscillator at the current view index.
 * @param[in] view Bar series view.
 * @param[in] fast Fast EMA period.
 * @param[in] slow Slow EMA period.
 * @return Chaikin Oscillator value, or 0.0 when invalid.
 */
double cxta_chaikin(const cxta_series_bar_view* view, int fast, int slow);

#ifdef __cplusplus
}
#endif
