/**
 * @file vwap.h
 * @brief VWAP helpers.
 */

#pragma once

#include "../series/bar.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Cumulative VWAP state for incremental updates.
 */
typedef struct {
    double pv_sum; /**< Running price-volume sum. */
    double v_sum;  /**< Running volume sum. */
} cxta_vwap_state;

/**
 * @brief Perform one cumulative VWAP update.
 * @param[in] typical_price Current typical price.
 * @param[in] volume Current volume.
 * @param[in,out] st VWAP state.
 * @return Updated cumulative VWAP value.
 */
double cxta_vwap_step(double typical_price, double volume, cxta_vwap_state* st);

/**
 * @brief Compute VWAP at the current view index.
 *
 * When @p period is `<= 0`, computes cumulative VWAP from bar 0.
 * Otherwise computes rolling-window VWAP over the last @p period bars.
 *
 * @param[in] view Bar series view.
 * @param[in] period Rolling window period, or `<= 0` for cumulative VWAP.
 * @return VWAP value, or 0.0 when invalid.
 */
double cxta_vwap(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
