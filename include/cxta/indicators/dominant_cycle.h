/**
 * @file dominant_cycle.h
 * @brief Dominant cycle period helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"

static const cxta_param_descriptor cxta_dominant_cycle_period_params[] = {
    {"min_period"},
    {"max_period"},
};

/** Defaults match `cxta_dominant_cycle_period_desc_eval` in `dominant_cycle.c`. */
static const cxta_expr_arg_descriptor cxta_dominant_cycle_period_expr_args[] = {
    {"min_period", CXTA_EXPR_ARG_NUMERIC, "10"},
    {"max_period", CXTA_EXPR_ARG_NUMERIC, "40"},
};

static const cxta_bridge_fn_spec cxta_dominant_cycle_period_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC_EXPR(
        "dominant_cycle_period",
        0u,
        2u,
        cxta_dominant_cycle_period_params,
        cxta_dominant_cycle_period_expr_args,
        1);

extern const cxta_indicator_descriptor cxta_dominant_cycle_period_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute dominant cycle period at the current view index.
 * @param[in] view Bar series view.
 * @param[in] min_period Minimum cycle period to consider.
 * @param[in] max_period Maximum cycle period to consider.
 * @return Dominant cycle period estimate, or 0.0 when invalid.
 */
double cxta_dominant_cycle(const cxta_series_bar_view* view,
                           int min_period,
                           int max_period);

#ifdef __cplusplus
}
#endif
