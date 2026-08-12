/**
 * @file atr.h
 * @brief Average True Range helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"

/** @brief Named parameters for the ATR indicator. */
static const cxta_param_descriptor cxta_atr_params[] = {
    {"period"},
};

static const cxta_expr_arg_descriptor cxta_atr_expr_args[] = {
    {"source", CXTA_EXPR_ARG_SCALAR_SOURCE, "bars", "Optional OHLC bar-series source."},
    {"period", CXTA_EXPR_ARG_NUMERIC, "14", "ATR smoothing period."},
};

/** @brief Bridge-facing signature metadata for ATR. */
static const cxta_bridge_fn_spec cxta_atr_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC_EXPR("atr", 1u, 1u, cxta_atr_params, cxta_atr_expr_args, 1);

/** @brief Expression-facing descriptor for ATR. */
extern const cxta_indicator_descriptor cxta_atr_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ATR state for incremental updates.
 */
typedef struct {
    double value;
    double prev_close;
    double initialized;
} cxta_atr_state;

/**
 * @brief Perform one ATR update from OHLC data.
 * @param[in] high Current high.
 * @param[in] low Current low.
 * @param[in] close Current close.
 * @param[in] period ATR period.
 * @param[in,out] st ATR state.
 * @return Updated ATR value.
 */
double cxta_atr_step(double high, double low, double close, int period, cxta_atr_state* st);

/**
 * @brief Compute ATR at the current view index.
 * @param[in] view Bar series view.
 * @param[in] period ATR period.
 * @return ATR value, or 0.0 when the view is invalid.
 */
double cxta_atr(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
