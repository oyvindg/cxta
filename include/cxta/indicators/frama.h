/**
 * @file frama.h
 * @brief FRAMA helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"

/** @brief Named parameters for the FRAMA indicator. */
static const cxta_param_descriptor cxta_frama_params[] = {
    {"period"},
};

/** @brief Bridge-facing signature metadata for FRAMA. */
static const cxta_bridge_fn_spec cxta_frama_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("frama", 1u, 1u, cxta_frama_params, 1);

/** @brief Expression-facing descriptor for FRAMA. */
extern const cxta_indicator_descriptor cxta_frama_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute FRAMA at the current view index.
 * @param[in] view Bar series view.
 * @param[in] period Fractal window period.
 * @return FRAMA value, or 0.0 when invalid.
 */
double cxta_frama(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
