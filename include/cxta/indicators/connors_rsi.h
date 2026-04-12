/**
 * @file connors_rsi.h
 * @brief Connors RSI helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"

static const cxta_param_descriptor cxta_crsi_params[] = {
    {"rsi_period"},
    {"streak_period"},
    {"rank_period"},
};

/** Defaults match `cxta_crsi_desc_eval` in `connors_rsi.c`. */
static const cxta_expr_arg_descriptor cxta_crsi_expr_args[] = {
    {"rsi_period", CXTA_EXPR_ARG_NUMERIC, "3"},
    {"streak_period", CXTA_EXPR_ARG_NUMERIC, "2"},
    {"rank_period", CXTA_EXPR_ARG_NUMERIC, "100"},
};

static const cxta_bridge_fn_spec cxta_crsi_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC_EXPR("crsi", 0u, 3u, cxta_crsi_params, cxta_crsi_expr_args, 1);

extern const cxta_indicator_descriptor cxta_crsi_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute Connors RSI at the current view index.
 * @param[in] view Bar series view.
 * @param[in] rsi_period RSI period for close.
 * @param[in] streak_period RSI period for streak.
 * @param[in] rank_period Percent-rank lookback.
 * @return Connors RSI in [0, 100], or 50.0 when invalid.
 */
double cxta_connors_rsi(const cxta_series_bar_view* view,
                        int rsi_period,
                        int streak_period,
                        int rank_period);

#ifdef __cplusplus
}
#endif
