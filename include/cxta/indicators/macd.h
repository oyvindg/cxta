/**
 * @file macd.h
 * @brief Moving Average Convergence Divergence helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"
#include "../indicators/macd_math.h"

/** @brief Named parameters for the MACD indicator. */
static const cxta_param_descriptor cxta_macd_params[] = {
    {"fast"},
    {"slow"},
    {"signal"},
};

static const cxta_expr_arg_descriptor cxta_macd_expr_args[] = {
    {"source", CXTA_EXPR_ARG_SCALAR_SOURCE, "close", "Optional input price/series used to compute the fast and slow EMAs; implicit when omitted."},
    {"fast", CXTA_EXPR_ARG_NUMERIC, "12", "Fast EMA period. Lower values make MACD react faster."},
    {"slow", CXTA_EXPR_ARG_NUMERIC, "26", "Slow EMA period. Higher values smooth the baseline trend more."},
    {"signal", CXTA_EXPR_ARG_NUMERIC, "9", "Signal EMA period applied to the MACD line."},
};

/** @brief Bridge-facing signature metadata for MACD. */
static const cxta_bridge_fn_spec cxta_macd_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC_EXPR("macd", 3u, 3u, cxta_macd_params, cxta_macd_expr_args, 1);

/** @brief Expression-facing descriptor for MACD. */
extern const cxta_indicator_descriptor cxta_macd_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Perform one MACD update.
 * @param[in] close Current close.
 * @param[in] fast Fast EMA period.
 * @param[in] slow Slow EMA period.
 * @param[in] signal Signal EMA period.
 * @param[in,out] st MACD state.
 * @return MACD outputs.
 */
cxta_macd_output cxta_macd_step(double close,
                                int fast,
                                int slow,
                                int signal,
                                cxta_macd_state* st);

/**
 * @brief Compute MACD at the current view index.
 * @param[in] view Bar series view.
 * @param[in] fast Fast EMA period.
 * @param[in] slow Slow EMA period.
 * @param[in] signal Signal EMA period.
 * @return MACD outputs.
 */
cxta_macd_output cxta_macd(const cxta_series_bar_view* view,
                           int fast,
                           int slow,
                           int signal);

#ifdef __cplusplus
}
#endif
