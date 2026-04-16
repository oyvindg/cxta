/**
 * @file pivots.h
 * @brief Expression-facing descriptors for pivot-derived structure indicators.
 */

#pragma once

#include <cxta/indicators/descriptor.h>

#ifdef __cplusplus
extern "C" {
#endif

extern const cxta_indicator_descriptor cxta_swing_pivots_descriptor;
extern const cxta_indicator_descriptor cxta_pivot_points_descriptor;
extern const cxta_indicator_descriptor cxta_sfp_descriptor;

extern const cxta_bridge_fn_spec cxta_pivot_points_bridge_fn_spec;
extern const cxta_bridge_fn_spec cxta_swing_pivots_bridge_fn_spec;
extern const cxta_bridge_fn_spec cxta_sfp_bridge_fn_spec;

#ifdef __cplusplus
}
#endif
