/**
 * @file structure.h
 * @brief Expression-facing descriptors for market-structure indicators (Wave 4).
 */

#pragma once

#include <cxta/indicators/descriptor.h>

#ifdef __cplusplus
extern "C" {
#endif

extern const cxta_indicator_descriptor cxta_swing_pivots_descriptor;
extern const cxta_indicator_descriptor cxta_pivot_points_descriptor;
extern const cxta_indicator_descriptor cxta_structure_descriptor;
extern const cxta_indicator_descriptor cxta_bos_descriptor;
extern const cxta_indicator_descriptor cxta_fvg_descriptor;
extern const cxta_indicator_descriptor cxta_order_block_descriptor;
extern const cxta_indicator_descriptor cxta_liquidity_descriptor;
extern const cxta_indicator_descriptor cxta_liquidity_pools_descriptor;
extern const cxta_indicator_descriptor cxta_sfp_descriptor;
extern const cxta_indicator_descriptor cxta_swing_anchor_vwap_descriptor;
extern const cxta_indicator_descriptor cxta_wedge_descriptor;
extern const cxta_indicator_descriptor cxta_broadening_descriptor;

extern const cxta_bridge_fn_spec cxta_pivot_points_bridge_fn_spec;
extern const cxta_bridge_fn_spec cxta_swing_pivots_bridge_fn_spec;
extern const cxta_bridge_fn_spec cxta_structure_bridge_fn_spec;
extern const cxta_bridge_fn_spec cxta_bos_bridge_fn_spec;
extern const cxta_bridge_fn_spec cxta_fvg_bridge_fn_spec;
extern const cxta_bridge_fn_spec cxta_order_block_bridge_fn_spec;
extern const cxta_bridge_fn_spec cxta_liquidity_bridge_fn_spec;
extern const cxta_bridge_fn_spec cxta_liquidity_pools_bridge_fn_spec;
extern const cxta_bridge_fn_spec cxta_sfp_bridge_fn_spec;
extern const cxta_bridge_fn_spec cxta_swing_anchor_vwap_bridge_fn_spec;
extern const cxta_bridge_fn_spec cxta_wedge_bridge_fn_spec;
extern const cxta_bridge_fn_spec cxta_broadening_bridge_fn_spec;

#ifdef __cplusplus
}
#endif
