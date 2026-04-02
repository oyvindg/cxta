/**
 * @file volume_ema.h
 * @brief Volume EMA helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Volume EMA state for incremental updates.
 */
typedef struct {
    double value;       /**< Current EMA value. */
    double initialized; /**< Non-zero once seeded. */
} cxta_volume_ema_state;

/**
 * @brief Perform one volume EMA update.
 * @param[in]     volume Current volume.
 * @param[in]     period EMA period.
 * @param[in,out] st     Volume EMA state.
 * @return Updated volume EMA value.
 */
double cxta_volume_ema_step(double volume, int period, cxta_volume_ema_state* st);

/**
 * @brief Compute volume-based EMA at the current view index.
 * @param[in] view   Bar series view.
 * @param[in] period EMA period.
 * @return Volume EMA value, or 0.0 when the view is invalid.
 */
double cxta_volume_ema(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
