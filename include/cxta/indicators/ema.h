/**
 * @file ema.h
 * @brief Exponential moving average helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"

/** @brief Named parameters for the EMA indicator. */
static const cxta_param_descriptor cxta_ema_params[] = {
    {"period"},
};

static const cxta_expr_arg_descriptor cxta_ema_expr_args[] = {
    {"source", CXTA_EXPR_ARG_SCALAR_SOURCE, "close", "Optional input price/series to smooth; implicit close when omitted."},
    {"period", CXTA_EXPR_ARG_NUMERIC, "20", "EMA smoothing period. Lower values react faster; higher values smooth more noise."},
};

/** @brief Bridge-facing signature metadata for EMA. */
static const cxta_bridge_fn_spec cxta_ema_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC_EXPR("ema", 1u, 1u, cxta_ema_params, cxta_ema_expr_args, 1);

/** @brief Expression-facing descriptor for EMA. */
extern const cxta_indicator_descriptor cxta_ema_descriptor;

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
