/**
 * @file bollinger.h
 * @brief Bollinger band helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"
#include "../indicators/bollinger_math.h"

/** @brief Named parameters for the Bollinger indicator. */
static const cxta_param_descriptor cxta_bollinger_params[] = {
    {"period"},
    {"stddev_mult"},
};

static const cxta_expr_arg_descriptor cxta_bollinger_expr_args[] = {
    {"source", CXTA_EXPR_ARG_SCALAR_SOURCE, "close"},
    {"period", CXTA_EXPR_ARG_NUMERIC, "20"},
    {"stddev_mult", CXTA_EXPR_ARG_NUMERIC, "2.0"},
};

/** @brief Bridge-facing signature metadata for Bollinger. */
static const cxta_bridge_fn_spec cxta_bollinger_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC_EXPR("bollinger", 2u, 2u, cxta_bollinger_params, cxta_bollinger_expr_args, 1);

/** @brief Expression-facing descriptor for Bollinger. */
extern const cxta_indicator_descriptor cxta_bollinger_descriptor;
/** @brief Default GUI plot metadata for Bollinger. */
extern const cxta_indicator_plot_descriptor cxta_bollinger_plot_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute Bollinger bands at the current view index.
 * @param[in] view Bar series view.
 * @param[in] period Lookback period.
 * @param[in] stddev_mult Standard-deviation multiplier.
 * @return Bollinger outputs.
 */
cxta_bollinger_output cxta_bollinger(const cxta_series_bar_view* view,
                                     int period,
                                     double stddev_mult);

#ifdef __cplusplus
}
#endif
