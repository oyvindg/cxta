/**
 * @file trend.h
 * @brief Expression-facing descriptors for trend structure indicators.
 */

#pragma once

#include <cxta/indicators/descriptor.h>

#ifdef __cplusplus
extern "C" {
#endif

extern const cxta_indicator_descriptor cxta_structure_descriptor;
extern const cxta_indicator_descriptor cxta_bos_descriptor;

extern const cxta_bridge_fn_spec cxta_structure_bridge_fn_spec;
extern const cxta_bridge_fn_spec cxta_bos_bridge_fn_spec;

#ifdef __cplusplus
}
#endif
