/**
 * @file donchian.h
 * @brief Donchian channel helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"

/** @brief Named parameters for Donchian channels. */
static const cxta_param_descriptor cxta_donchian_params[] = {
    {"period"},
};

/** @brief Bridge-facing signature metadata for Donchian. */
static const cxta_bridge_fn_spec cxta_donchian_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("donchian", 1u, 1u, cxta_donchian_params, 1);

/** @brief Expression-facing descriptor for Donchian. */
extern const cxta_indicator_descriptor cxta_donchian_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Generic channel output payload.
 */
typedef struct {
    double upper;
    double lower;
    double middle;
    double width;
} cxta_channel_output;

/**
 * @brief Compute a Donchian channel at the current view index.
 * @param[in] view Bar series view.
 * @param[in] period Lookback period.
 * @return Channel outputs.
 */
cxta_channel_output cxta_donchian(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif