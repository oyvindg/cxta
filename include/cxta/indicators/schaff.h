/**
 * @file schaff.h
 * @brief Schaff Trend Cycle helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"

static const cxta_param_descriptor cxta_schaff_trend_cycle_params[] = {
    {"fast_period"},
    {"slow_period"},
    {"cycle_period"},
};
static const cxta_bridge_fn_spec cxta_schaff_trend_cycle_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("schaff_trend_cycle", 3u, 3u, cxta_schaff_trend_cycle_params, 1);

extern const cxta_indicator_descriptor cxta_schaff_trend_cycle_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute Schaff Trend Cycle at the current view index.
 * @param[in] view Bar series view.
 * @param[in] fast Fast EMA period.
 * @param[in] slow Slow EMA period.
 * @param[in] cycle Cycle period.
 * @return STC value, or 0.0 when invalid.
 */
double cxta_schaff(const cxta_series_bar_view* view, int fast, int slow, int cycle);

#ifdef __cplusplus
}
#endif
