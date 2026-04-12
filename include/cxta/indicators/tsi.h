/**
 * @file tsi.h
 * @brief True Strength Index helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"

/** @brief Named parameters for the TSI indicator. */
static const cxta_param_descriptor cxta_tsi_params[] = {
    {"long_period"},
    {"short_period"},
};

/** @brief Bridge-facing signature metadata for TSI. */
static const cxta_bridge_fn_spec cxta_tsi_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("tsi", 2u, 2u, cxta_tsi_params, 1);

/** @brief Expression-facing descriptor for TSI. */
extern const cxta_indicator_descriptor cxta_tsi_descriptor;

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
