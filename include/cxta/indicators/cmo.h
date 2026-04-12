/**
 * @file cmo.h
 * @brief Chande Momentum Oscillator helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"

static const cxta_param_descriptor cxta_cmo_params[] = {
    {"period"},
};
static const cxta_bridge_fn_spec cxta_cmo_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("cmo", 1u, 1u, cxta_cmo_params, 1);

extern const cxta_indicator_descriptor cxta_cmo_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute CMO at the current view index.
 * @param[in] view Bar series view.
 * @param[in] period Lookback period.
 * @return CMO value in [-100, 100], or 0.0 when invalid.
 */
double cxta_cmo(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
