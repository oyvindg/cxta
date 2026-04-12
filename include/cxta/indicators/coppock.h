/**
 * @file coppock.h
 * @brief Coppock Curve helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"

static const cxta_param_descriptor cxta_coppock_curve_params[] = {
    {"wma_period"},
    {"long_roc_period"},
    {"short_roc_period"},
};

/** Defaults match `cxta_coppock_curve_desc_eval` in `coppock.c`. */
static const cxta_expr_arg_descriptor cxta_coppock_curve_expr_args[] = {
    {"wma_period", CXTA_EXPR_ARG_NUMERIC, "10"},
    {"long_roc_period", CXTA_EXPR_ARG_NUMERIC, "14"},
    {"short_roc_period", CXTA_EXPR_ARG_NUMERIC, "11"},
};

static const cxta_bridge_fn_spec cxta_coppock_curve_bridge_fn_spec = CXTA_BRIDGE_FN_SPEC_EXPR(
    "coppock_curve",
    0u,
    3u,
    cxta_coppock_curve_params,
    cxta_coppock_curve_expr_args,
    1);

extern const cxta_indicator_descriptor cxta_coppock_curve_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute Coppock Curve at the current view index.
 * @param[in] view Bar series view.
 * @param[in] wma_period WMA period over the ROC sum.
 * @param[in] long_roc Long ROC period.
 * @param[in] short_roc Short ROC period.
 * @return Coppock Curve value, or 0.0 when invalid.
 */
double cxta_coppock(const cxta_series_bar_view* view,
                    int wma_period,
                    int long_roc,
                    int short_roc);

#ifdef __cplusplus
}
#endif
