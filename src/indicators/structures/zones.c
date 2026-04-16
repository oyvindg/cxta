/**
 * @file zones.c
 * @brief Descriptor metadata for structure zone indicators.
 */

#include "common.h"

#include <cxta/indicators/structures/zones.h>
#include <cxta/structure/fvg.h>
#include <cxta/structure/liquidity.h>
#include <cxta/structure/order_block.h>
#include <cxta/structure/pivot.h>

static const cxta_field_descriptor cxta_fvg_fields[] = {
    CXTA_STRUCT_FIELD_HIDDEN("bullish", cxta_struct_fvg_state, bullish),
    CXTA_STRUCT_FIELD_HIDDEN("bearish", cxta_struct_fvg_state, bearish),
    CXTA_STRUCT_FIELD("top", cxta_struct_fvg_state, top),
    CXTA_STRUCT_FIELD("bottom", cxta_struct_fvg_state, bottom),
    CXTA_STRUCT_FIELD("filled", cxta_struct_fvg_state, filled),
};

static const cxta_field_descriptor cxta_order_block_fields[] = {
    CXTA_STRUCT_FIELD("bullish", cxta_struct_order_block_state, bullish),
    CXTA_STRUCT_FIELD("bearish", cxta_struct_order_block_state, bearish),
    CXTA_STRUCT_FIELD("top", cxta_struct_order_block_state, top),
    CXTA_STRUCT_FIELD("bottom", cxta_struct_order_block_state, bottom),
    CXTA_STRUCT_FIELD("strength", cxta_struct_order_block_state, strength),
};

static const cxta_field_descriptor cxta_liquidity_fields[] = {
    CXTA_STRUCT_FIELD_HIDDEN("buy_side", cxta_struct_liquidity_state, buy_side),
    CXTA_STRUCT_FIELD_HIDDEN("sell_side", cxta_struct_liquidity_state, sell_side),
    CXTA_STRUCT_FIELD("level", cxta_struct_liquidity_state, level),
    CXTA_STRUCT_FIELD("swept", cxta_struct_liquidity_state, swept),
};

static void cxta_fvg_descriptor_eval(const cxta_series_bar_view* view,
                                     const double* args,
                                     size_t nargs,
                                     void* out) {
    cxta_struct_fvg_state value = {0};

    if (view && cxta_series_bar_view_valid(view)) {
        const int fallback = (int)cxta_series_clamp_index(view->size, view->index) + 1;
        const size_t lookback =
            (size_t)cxta_descriptor_period_arg(args, nargs, 0u, fallback);
        (void)cxta_struct_fvg_state_compute(view, lookback, &value);
    }
    cxta_descriptor_copy_struct(out, &value, sizeof(value));
}

static void cxta_order_block_descriptor_eval(const cxta_series_bar_view* view,
                                             const double* args,
                                             size_t nargs,
                                             void* out) {
    cxta_struct_order_block_state value = {0};
    cxta_struct_pivot_args parsed = {0};

    if (view && cxta_descriptor_parse_pivot_args(view, args, nargs, &parsed)) {
        (void)cxta_struct_order_block_state_compute(view, &parsed, &value);
    }
    cxta_descriptor_copy_struct(out, &value, sizeof(value));
}

static void cxta_liquidity_descriptor_eval(const cxta_series_bar_view* view,
                                           const double* args,
                                           size_t nargs,
                                           void* out) {
    cxta_struct_pivot_state pivot = {0};
    cxta_struct_liquidity_state value = {0};
    cxta_struct_pivot_args parsed = {0};
    const double tolerance = cxta_descriptor_double_arg(args, nargs, 3u, 0.001);

    if (view && cxta_descriptor_parse_pivot_args(view, args, nargs, &parsed)) {
        (void)cxta_struct_pivot_state_compute(view, &parsed, &pivot);
        (void)cxta_struct_liquidity_state_compute(view, &pivot, tolerance, &value);
    }
    cxta_descriptor_copy_struct(out, &value, sizeof(value));
}

static const cxta_param_descriptor cxta_fvg_params[] = {
    {"lookback"},
};

static const cxta_param_descriptor cxta_order_block_params[] = {
    {"left"},
    {"right"},
    {"lookback"},
};

static const cxta_param_descriptor cxta_liquidity_params[] = {
    {"left"},
    {"right"},
    {"lookback"},
    {"tolerance"},
};

const cxta_bridge_fn_spec cxta_fvg_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("fvg", 0u, 1u, cxta_fvg_params, 1);
const cxta_bridge_fn_spec cxta_order_block_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("order_block", 2u, 3u, cxta_order_block_params, 1);
const cxta_bridge_fn_spec cxta_liquidity_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("liquidity", 2u, 4u, cxta_liquidity_params, 1);

const cxta_indicator_descriptor cxta_fvg_descriptor =
    CXTA_STRUCTURE_DESCRIPTOR("fvg",
                              0,
                              1,
                              0,
                              cxta_struct_fvg_state,
                              0u,
                              cxta_fvg_fields,
                              cxta_fvg_descriptor_eval,
                              NULL,
                              cxta_fvg_params);

const cxta_indicator_descriptor cxta_order_block_descriptor =
    CXTA_STRUCTURE_DESCRIPTOR("order_block",
                              2,
                              3,
                              0,
                              cxta_struct_order_block_state,
                              0u,
                              cxta_order_block_fields,
                              cxta_order_block_descriptor_eval,
                              NULL,
                              cxta_order_block_params);

const cxta_indicator_descriptor cxta_liquidity_descriptor =
    CXTA_STRUCTURE_DESCRIPTOR("liquidity",
                              2,
                              4,
                              0,
                              cxta_struct_liquidity_state,
                              0u,
                              cxta_liquidity_fields,
                              cxta_liquidity_descriptor_eval,
                              NULL,
                              cxta_liquidity_params);
