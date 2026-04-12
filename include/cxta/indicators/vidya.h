/**
 * @file vidya.h
 * @brief VIDYA helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"

/** @brief Named parameters for the VIDYA indicator. */
static const cxta_param_descriptor cxta_vidya_params[] = {
    {"period"},
};

/** @brief Bridge-facing signature metadata for VIDYA. */
static const cxta_bridge_fn_spec cxta_vidya_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("vidya", 1u, 1u, cxta_vidya_params, 1);

/** @brief Expression-facing descriptor for VIDYA. */
extern const cxta_indicator_descriptor cxta_vidya_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute VIDYA at the current view index.
 * @param[in] view Bar series view.
 * @param[in] period VIDYA base period.
 * @return VIDYA value, or 0.0 when invalid.
 */
double cxta_vidya(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
