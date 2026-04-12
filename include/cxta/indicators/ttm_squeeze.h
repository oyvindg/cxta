/**
 * @file ttm_squeeze.h
 * @brief TTM squeeze helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"

static const cxta_param_descriptor cxta_ttm_squeeze_params[] = {
    {"bb_period"},
    {"bb_mult"},
    {"kc_period"},
    {"kc_mult"},
    {"momentum_period"},
};

/** Defaults match `cxta_ttm_squeeze_descriptor_eval` fallbacks in `ttm_squeeze.c`. */
static const cxta_expr_arg_descriptor cxta_ttm_squeeze_expr_args[] = {
    {"bb_period", CXTA_EXPR_ARG_NUMERIC, "20"},
    {"bb_mult", CXTA_EXPR_ARG_NUMERIC, "2"},
    {"kc_period", CXTA_EXPR_ARG_NUMERIC, "20"},
    {"kc_mult", CXTA_EXPR_ARG_NUMERIC, "1.5"},
    {"momentum_period", CXTA_EXPR_ARG_NUMERIC, "20"},
};

static const cxta_bridge_fn_spec cxta_ttm_squeeze_bridge_fn_spec = CXTA_BRIDGE_FN_SPEC_EXPR(
    "ttm_squeeze",
    0u,
    5u,
    cxta_ttm_squeeze_params,
    cxta_ttm_squeeze_expr_args,
    1);

extern const cxta_indicator_descriptor cxta_ttm_squeeze_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief TTM squeeze output payload.
 */
typedef struct {
    double squeeze_on;
    double squeeze_off;
    double momentum;
} cxta_squeeze_output;

/**
 * @brief Compute TTM squeeze outputs at the current view index.
 * @param[in] view Bar series view.
 * @param[in] bb_period Bollinger period.
 * @param[in] bb_mult Bollinger standard-deviation multiplier.
 * @param[in] kc_period Keltner EMA/ATR period.
 * @param[in] kc_mult Keltner ATR multiplier.
 * @param[in] momentum_period Momentum SMA lookback.
 * @return TTM squeeze outputs, or a zero-initialized struct when invalid.
 */
cxta_squeeze_output cxta_ttm_squeeze(const cxta_series_bar_view* view,
                                     int bb_period,
                                     double bb_mult,
                                     int kc_period,
                                     double kc_mult,
                                     int momentum_period);

#ifdef __cplusplus
}
#endif
