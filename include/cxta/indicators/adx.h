/**
 * @file adx.h
 * @brief Average Directional Index helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"
#include "../indicators/adx_math.h"

/** @brief Named parameters for ADX. */
static const cxta_param_descriptor cxta_adx_params[] = {
    {"period"},
};

static const cxta_expr_arg_descriptor cxta_adx_expr_args[] = {
    {"source", CXTA_EXPR_ARG_SCALAR_SOURCE, "bars", "Optional OHLC bar-series source."},
    {"period", CXTA_EXPR_ARG_NUMERIC, "14", "ADX smoothing period."},
};

/** @brief Bridge-facing signature metadata for ADX. */
static const cxta_bridge_fn_spec cxta_adx_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC_EXPR("adx", 1u, 1u, cxta_adx_params, cxta_adx_expr_args, 1);

/** @brief Expression-facing descriptor for ADX. */
extern const cxta_indicator_descriptor cxta_adx_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Perform one ADX update.
 * @param[in] plus_dm Positive directional movement sample.
 * @param[in] minus_dm Negative directional movement sample.
 * @param[in] tr True range sample.
 * @param[in] period ADX period.
 * @param[in,out] st ADX state.
 * @return ADX outputs.
 */
cxta_adx_output cxta_adx_step(double plus_dm,
                              double minus_dm,
                              double tr,
                              int period,
                              cxta_adx_state* st);

/**
 * @brief Compute ADX at the current view index.
 * @param[in] view Bar series view.
 * @param[in] period ADX period.
 * @return ADX outputs.
 */
cxta_adx_output cxta_adx(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
