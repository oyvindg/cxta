/**
 * @file roc.h
 * @brief Rate of Change helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"

/** @brief Named parameters for the ROC indicator. */
static const cxta_param_descriptor cxta_roc_params[] = {
    {"period"},
};

/** @brief Bridge-facing signature metadata for ROC. */
static const cxta_bridge_fn_spec cxta_roc_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("roc", 1u, 1u, cxta_roc_params, 1);

/** @brief Expression-facing descriptor for ROC. */
extern const cxta_indicator_descriptor cxta_roc_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute Rate of Change at the current view index.
 *
 * ROC = (close / close[n bars ago] - 1) * 100.
 *
 * @param[in] view   Bar series view.
 * @param[in] period Lookback period in bars.
 * @return ROC value in percent, or 0.0 when the view is invalid or the
 *         reference bar has a close of 0.
 */
double cxta_roc(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
