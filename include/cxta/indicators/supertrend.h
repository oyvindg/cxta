/**
 * @file supertrend.h
 * @brief Supertrend helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"

static const cxta_param_descriptor cxta_supertrend_params[] = {
    {"period"},
    {"mult"},
};

static const cxta_expr_arg_descriptor cxta_supertrend_expr_args[] = {
    {"period", CXTA_EXPR_ARG_NUMERIC, "10", "ATR lookback period used to build the Supertrend bands."},
    {"mult", CXTA_EXPR_ARG_NUMERIC, "3.0", "ATR multiplier applied to the midpoint band distance."},
};

static const cxta_bridge_fn_spec cxta_supertrend_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC_EXPR("supertrend", 2u, 2u, cxta_supertrend_params, cxta_supertrend_expr_args, 1);

extern const cxta_indicator_descriptor cxta_supertrend_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Supertrend output payload.
 */
typedef struct {
    double value;
    double direction;
    double strength;
} cxta_supertrend_output;

/**
 * @brief Supertrend state for incremental updates.
 */
typedef struct {
    double atr;
    double final_upper;
    double final_lower;
    double direction;
} cxta_supertrend_state;

/**
 * @brief Perform one Supertrend update.
 * @param[in] high Current high.
 * @param[in] low Current low.
 * @param[in] close Current close.
 * @param[in] prev_close Previous close.
 * @param[in] period ATR period.
 * @param[in] multiplier Band multiplier.
 * @param[in,out] st Supertrend state.
 * @return Supertrend outputs.
 */
cxta_supertrend_output cxta_supertrend_step(double high,
                                            double low,
                                            double close,
                                            double prev_close,
                                            int period,
                                            double multiplier,
                                            cxta_supertrend_state* st);

/**
 * @brief Compute Supertrend at the current view index.
 * @param[in] view Bar series view.
 * @param[in] period ATR period.
 * @param[in] multiplier Band multiplier.
 * @return Supertrend outputs.
 */
cxta_supertrend_output cxta_supertrend(const cxta_series_bar_view* view,
                                       int period,
                                       double multiplier);

#ifdef __cplusplus
}
#endif
