/**
 * @file price_channel.h
 * @brief Price channel helpers.
 */

#pragma once

#include "donchian.h"

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
