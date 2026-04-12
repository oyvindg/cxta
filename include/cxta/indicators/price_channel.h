/**
 * @file price_channel.h
 * @brief Price channel helpers.
 */

#pragma once

#include "donchian.h"
#include "../indicators/descriptor.h"

/** @brief Named parameters for price channel. */
static const cxta_param_descriptor cxta_price_channel_params[] = {
    {"period"},
};

/** @brief Bridge-facing signature metadata for price channel. */
static const cxta_bridge_fn_spec cxta_price_channel_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("price_channel", 1u, 1u, cxta_price_channel_params, 1);

/** @brief Expression-facing descriptor for price channel. */
extern const cxta_indicator_descriptor cxta_price_channel_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute a price channel at the current view index.
 * @param[in] view Bar series view.
 * @param[in] period Lookback period.
 * @return Channel outputs.
 */
cxta_channel_output cxta_price_channel(const cxta_series_bar_view* view, int period);

#ifdef __cplusplus
}
#endif
