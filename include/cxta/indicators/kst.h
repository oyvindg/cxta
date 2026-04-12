/**
 * @file kst.h
 * @brief KST helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"

static const cxta_param_descriptor cxta_kst_params[] = {
    {"roc1_period"},
    {"roc2_period"},
    {"roc3_period"},
    {"roc4_period"},
};

/** Defaults match `CXTA_WRAP_BAR_SCALAR_4I(..., 10, 15, 20, 30)` in `kst.c`. */
static const cxta_expr_arg_descriptor cxta_kst_expr_args[] = {
    {"roc1_period", CXTA_EXPR_ARG_NUMERIC, "10"},
    {"roc2_period", CXTA_EXPR_ARG_NUMERIC, "15"},
    {"roc3_period", CXTA_EXPR_ARG_NUMERIC, "20"},
    {"roc4_period", CXTA_EXPR_ARG_NUMERIC, "30"},
};

static const cxta_bridge_fn_spec cxta_kst_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC_EXPR("kst", 0u, 4u, cxta_kst_params, cxta_kst_expr_args, 1);

extern const cxta_indicator_descriptor cxta_kst_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute KST at the current view index.
 * @param[in] view Bar series view.
 * @param[in] p1 First ROC period.
 * @param[in] p2 Second ROC period.
 * @param[in] p3 Third ROC period.
 * @param[in] p4 Fourth ROC period.
 * @return KST value, or 0.0 when invalid.
 */
double cxta_kst(const cxta_series_bar_view* view, int p1, int p2, int p3, int p4);

#ifdef __cplusplus
}
#endif
