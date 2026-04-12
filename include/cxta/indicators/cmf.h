/**
 * @file cmf.h
 * @brief Chaikin Money Flow helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"

static const cxta_param_descriptor cxta_cmf_params[] = {
    {"period"},
};
static const cxta_bridge_fn_spec cxta_cmf_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("cmf", 1u, 1u, cxta_cmf_params, 1);

extern const cxta_indicator_descriptor cxta_cmf_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute Chaikin Money Flow at the current view index.
 * @param[in] view Bar series view.
 * @param[in] period Lookback period.
 * @return CMF value, or 0.0 when invalid.
 */
double cxta_cmf(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
