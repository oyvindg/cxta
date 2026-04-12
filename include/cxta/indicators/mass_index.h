/**
 * @file mass_index.h
 * @brief Mass Index helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"

static const cxta_param_descriptor cxta_mass_index_params[] = {
    {"sum_period"},
    {"ema_period"},
};
static const cxta_bridge_fn_spec cxta_mass_index_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("mass_index", 1u, 2u, cxta_mass_index_params, 1);

extern const cxta_indicator_descriptor cxta_mass_index_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute Mass Index at the current view index.
 * @param[in] view Bar series view.
 * @param[in] sum_period Rolling ratio-sum period.
 * @param[in] ema_period EMA period for the range smoothing.
 * @return Mass Index value, or 0.0 when invalid.
 */
double cxta_mass_index(const cxta_series_bar_view* view, int sum_period, int ema_period);

#ifdef __cplusplus
}
#endif
