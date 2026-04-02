/**
 * @file trix.h
 * @brief TRIX helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief TRIX state for incremental updates.
 */
typedef struct {
    double ema1;        /**< First EMA state. */
    double ema2;        /**< Second EMA state. */
    double ema3;        /**< Third EMA state. */
    double prev_ema3;   /**< Previous triple EMA. */
    double initialized; /**< Non-zero once seeded. */
} cxta_trix_state;

/**
 * @brief Perform one TRIX update.
 * @param[in] close Current close.
 * @param[in] period EMA period.
 * @param[in,out] st TRIX state.
 * @return Updated TRIX value.
 */
double cxta_trix_step(double close, int period, cxta_trix_state* st);

/**
 * @brief Compute TRIX at the current view index.
 * @param[in] view Bar series view.
 * @param[in] period EMA period.
 * @return TRIX value, or 0.0 when invalid.
 */
double cxta_trix(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
