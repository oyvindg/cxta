/**
 * @file mama.h
 * @brief MAMA/FAMA helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"

static const cxta_param_descriptor cxta_mama_params[] = {
    {"fast_limit"},
    {"slow_limit"},
};

/** Defaults match `cxta_mama_descriptor_eval` in `mama.c` (before internal clamping). */
static const cxta_expr_arg_descriptor cxta_mama_expr_args[] = {
    {"fast_limit", CXTA_EXPR_ARG_NUMERIC, "0.5"},
    {"slow_limit", CXTA_EXPR_ARG_NUMERIC, "0.05"},
};

static const cxta_bridge_fn_spec cxta_mama_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC_EXPR("mama", 0u, 2u, cxta_mama_params, cxta_mama_expr_args, 1);

extern const cxta_indicator_descriptor cxta_mama_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief MAMA output payload.
 */
typedef struct {
    double mama; /**< Adaptive moving average. */
    double fama; /**< Following adaptive moving average. */
} cxta_mama_output;

/**
 * @brief Compute MAMA/FAMA outputs at the current view index.
 * @param[in] view Bar series view.
 * @param[in] fast_limit Fast alpha limit.
 * @param[in] slow_limit Slow alpha limit.
 * @return MAMA outputs, or a zero-initialized struct when invalid.
 */
cxta_mama_output cxta_mama(const cxta_series_bar_view* view,
                           double fast_limit,
                           double slow_limit);

#ifdef __cplusplus
}
#endif
