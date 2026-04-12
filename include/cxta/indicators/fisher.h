/**
 * @file fisher.h
 * @brief Fisher Transform helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"

static const cxta_param_descriptor cxta_fisher_transform_params[] = {
    {"period"},
};
static const cxta_bridge_fn_spec cxta_fisher_transform_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("fisher_transform", 1u, 1u, cxta_fisher_transform_params, 1);

extern const cxta_indicator_descriptor cxta_fisher_transform_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute Fisher Transform at the current view index.
 * @param[in] view Bar series view.
 * @param[in] period Lookback period.
 * @return Fisher Transform value, or 0.0 when invalid.
 */
double cxta_fisher(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
