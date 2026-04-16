/**
 * @file patterns.h
 * @brief Expression-facing descriptors for structure pattern indicators.
 */

#pragma once

#include <cxta/indicators/descriptor.h>

#ifdef __cplusplus
extern "C" {
#endif

extern const cxta_indicator_descriptor cxta_swing_anchor_vwap_descriptor;
extern const cxta_indicator_descriptor cxta_wedge_descriptor;

extern const cxta_bridge_fn_spec cxta_swing_anchor_vwap_bridge_fn_spec;
extern const cxta_bridge_fn_spec cxta_wedge_bridge_fn_spec;

#ifdef __cplusplus
}
#endif
