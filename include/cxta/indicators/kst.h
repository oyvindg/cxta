/**
 * @file kst.h
 * @brief KST helpers.
 */

#pragma once

#include "kst_math.h"
#include "../series/bar.h"
#include "../indicators/descriptor.h"

static const cxta_param_descriptor cxta_kst_params[] = {
    {"roc1_period"},
    {"roc2_period"},
    {"roc3_period"},
    {"roc4_period"},
    {"signal_period"},
};

static const cxta_expr_arg_descriptor cxta_kst_expr_args[] = {
    {"source", CXTA_EXPR_ARG_SCALAR_SOURCE, "close"},
    {"roc1_period", CXTA_EXPR_ARG_NUMERIC, "10"},
    {"roc2_period", CXTA_EXPR_ARG_NUMERIC, "15"},
    {"roc3_period", CXTA_EXPR_ARG_NUMERIC, "20"},
    {"roc4_period", CXTA_EXPR_ARG_NUMERIC, "30"},
    {"signal_period", CXTA_EXPR_ARG_NUMERIC, "9"},
};

static const cxta_bridge_fn_spec cxta_kst_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC_EXPR("kst", 5u, 5u, cxta_kst_params, cxta_kst_expr_args, 1);

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
 * @param[in] signal_period SMA period applied to the KST line.
 * @return KST outputs.
 */
cxta_kst_output cxta_kst(const cxta_series_bar_view* view,
                         int p1,
                         int p2,
                         int p3,
                         int p4,
                         int signal_period);

#ifdef __cplusplus
}
#endif
