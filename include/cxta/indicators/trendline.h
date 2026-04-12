/**
 * @file trendline.h
 * @brief Pivot-derived trendline helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"

static const cxta_param_descriptor cxta_trendline_params[] = {
    {"left"},
    {"right"},
};

static const cxta_bridge_fn_spec cxta_trendline_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("trendline", 2u, 2u, cxta_trendline_params, 1);

extern const cxta_indicator_descriptor cxta_trendline_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Trendline output payload.
 */
typedef struct {
    double upper;
    double lower;
    double upper_slope;
    double lower_slope;
} cxta_trendline_output;

/**
 * @brief Compute trendline outputs at the current view index.
 * @param[in] view Bar series view.
 * @param[in] left Pivot-left window.
 * @param[in] right Pivot-right window.
 * @return Trendline outputs, or a zero-initialized struct when invalid.
 */
cxta_trendline_output cxta_trendline(const cxta_series_bar_view* view,
                                     int left,
                                     int right);

#ifdef __cplusplus
}
#endif
