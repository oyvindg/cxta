/**
 * @file typical.h
 * @brief Typical price helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"

extern const cxta_bridge_fn_spec cxta_typical_price_bridge_fn_spec;
extern const cxta_indicator_descriptor cxta_typical_price_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute typical price (H+L+C)/3 for the current bar.
 * @param[in] view Bar series view.
 * @return Typical price, or 0.0 when the view is invalid.
 */
double cxta_typical(const cxta_series_bar_view* view);

#ifdef __cplusplus
}
#endif
