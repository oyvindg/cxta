/**
 * @file obv.h
 * @brief On Balance Volume helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"

extern const cxta_bridge_fn_spec cxta_obv_bridge_fn_spec;
extern const cxta_indicator_descriptor cxta_obv_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief OBV state for incremental updates.
 */
typedef struct {
    double value;       /**< Cumulative OBV. */
    double prev_close;  /**< Previous close for direction comparison. */
    double initialized; /**< Non-zero once seeded. */
} cxta_obv_state;

/**
 * @brief Perform one OBV update.
 * @param[in]     close   Current close.
 * @param[in]     volume  Current volume.
 * @param[in,out] st      OBV state.
 * @return Updated OBV value.
 */
double cxta_obv_step(double close, double volume, cxta_obv_state* st);

/**
 * @brief Compute cumulative OBV from bar 0 to the current view index.
 * @param[in] view Bar series view.
 * @return OBV value, or 0.0 when the view is invalid.
 */
double cxta_obv(const cxta_series_bar_view* view);

#ifdef __cplusplus
}
#endif
