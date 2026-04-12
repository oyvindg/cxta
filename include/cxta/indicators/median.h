/**
 * @file median.h
 * @brief Median price helpers.
 */

#pragma once

#include "../series/bar.h"
#include "../indicators/descriptor.h"

extern const cxta_bridge_fn_spec cxta_median_price_bridge_fn_spec;
extern const cxta_indicator_descriptor cxta_median_price_descriptor;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compute median price (H+L)/2 for the current bar.
 * @param[in] view Bar series view.
 * @return Median price, or 0.0 when the view is invalid.
 */
double cxta_median_price(const cxta_series_bar_view* view);

#ifdef __cplusplus
}
#endif
