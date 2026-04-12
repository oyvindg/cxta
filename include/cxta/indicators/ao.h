/**
 * @file ao.h
 * @brief Awesome Oscillator helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"

static const cxta_param_descriptor cxta_awesome_oscillator_params[] = {
    {"fast_period"},
    {"slow_period"},
};
static const cxta_bridge_fn_spec cxta_awesome_oscillator_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("awesome_oscillator", 2u, 2u, cxta_awesome_oscillator_params, 1);

extern const cxta_indicator_descriptor cxta_awesome_oscillator_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute the Awesome Oscillator at the current view index.
 *
 * AO = SMA(5, midpoint) − SMA(34, midpoint), where midpoint = (H + L) / 2.
 *
 * @param[in] view Bar series view.
 * @return AO value, or 0.0 when the view is invalid.
 */
double cxta_ao(const cxta_series_bar_view* view);

#ifdef __cplusplus
}
#endif
