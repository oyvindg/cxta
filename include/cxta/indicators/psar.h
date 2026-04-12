/**
 * @file psar.h
 * @brief Parabolic SAR helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"

static const cxta_param_descriptor cxta_parabolic_sar_params[] = {
    {"step"},
    {"max_step"},
};

/** Defaults match `CXTA_WRAP_BAR_STRUCT_2D(..., 0.02, 0.2)` in `psar.c`. */
static const cxta_expr_arg_descriptor cxta_parabolic_sar_expr_args[] = {
    {"step", CXTA_EXPR_ARG_NUMERIC, "0.02"},
    {"max_step", CXTA_EXPR_ARG_NUMERIC, "0.2"},
};

static const cxta_bridge_fn_spec cxta_parabolic_sar_bridge_fn_spec = CXTA_BRIDGE_FN_SPEC_EXPR(
    "parabolic_sar",
    0u,
    2u,
    cxta_parabolic_sar_params,
    cxta_parabolic_sar_expr_args,
    1);

extern const cxta_indicator_descriptor cxta_parabolic_sar_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Parabolic SAR output payload.
 */
typedef struct {
    double value;
    double direction;
} cxta_psar_output;

/**
 * @brief Incremental Parabolic SAR state.
 *
 * Reserved for future incremental consumers. The standalone replay helper
 * `cxta_psar()` does not require pre-seeded state.
 */
typedef struct {
    double sar;
    double ep;
    double af;
    double direction;
} cxta_psar_state;

/**
 * @brief Compute Parabolic SAR at the current view index.
 * @param[in] view Bar series view.
 * @param[in] step Acceleration step.
 * @param[in] max_step Maximum acceleration step.
 * @return Parabolic SAR outputs, or a zero-initialized struct when invalid.
 */
cxta_psar_output cxta_psar(const cxta_series_bar_view* view,
                           double step,
                           double max_step);

#ifdef __cplusplus
}
#endif
