/**
 * @file eom.h
 * @brief Ease of Movement helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"

static const cxta_param_descriptor cxta_ease_of_movement_params[] = {
    {"period"},
};
static const cxta_bridge_fn_spec cxta_ease_of_movement_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("ease_of_movement", 1u, 1u, cxta_ease_of_movement_params, 1);

extern const cxta_indicator_descriptor cxta_ease_of_movement_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute Ease of Movement for the current bar.
 *
 * EOM = ((H+L)/2 − prev(H+L)/2) × (H−L) / volume
 *
 * Returns 0.0 at the first bar, or when volume or bar range is zero.
 *
 * @param[in] view Bar series view.
 * @return EOM value, or 0.0 when the view is invalid or first bar.
 */
double cxta_eom(const cxta_series_bar_view* view);

#ifdef __cplusplus
}
#endif
