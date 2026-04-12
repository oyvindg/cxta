/**
 * @file choppiness.h
 * @brief Choppiness Index helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"

static const cxta_param_descriptor cxta_choppiness_index_params[] = {
    {"period"},
};
static const cxta_bridge_fn_spec cxta_choppiness_index_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("choppiness_index", 1u, 1u, cxta_choppiness_index_params, 1);

extern const cxta_indicator_descriptor cxta_choppiness_index_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute Choppiness Index at the current view index.
 * @param[in] view Bar series view.
 * @param[in] period Lookback period.
 * @return Choppiness Index, or 0.0 when invalid.
 */
double cxta_choppiness(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
