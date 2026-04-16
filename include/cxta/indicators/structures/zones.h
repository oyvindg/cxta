/**
 * @file zones.h
 * @brief Expression-facing descriptors for structure zone indicators.
 */

#pragma once

#include <cxta/indicators/descriptor.h>

#ifdef __cplusplus
extern "C" {
#endif

extern const cxta_indicator_descriptor cxta_fvg_descriptor;
extern const cxta_indicator_descriptor cxta_order_block_descriptor;
extern const cxta_indicator_descriptor cxta_liquidity_descriptor;

extern const cxta_bridge_fn_spec cxta_fvg_bridge_fn_spec;
extern const cxta_bridge_fn_spec cxta_order_block_bridge_fn_spec;
extern const cxta_bridge_fn_spec cxta_liquidity_bridge_fn_spec;

#ifdef __cplusplus
}
#endif
