/**
 * @file structure.c
 * @brief Expression-facing descriptors for market-structure indicators (Wave 4).
 */

#include <cxta/indicators/structure.h>
#include <cxta/indicators/descriptor.h>
#include <cxta/structure/bos.h>
#include <cxta/structure/fvg.h>
#include <cxta/structure/liquidity.h>
#include <cxta/structure/order_block.h>
#include <cxta/structure/pivot.h>
#include <cxta/structure/pivot_points.h>
#include <cxta/structure/structure.h>
#include <cxta/structure/sfp.h>
#include <cxta/structure/vwap.h>
#include <cxta/structure/wedge.h>

#include <stddef.h>
#include <string.h>

#define CXTA_FIELD(name, offset) {(name), (offset), true}
#define CXTA_FIELD_HIDDEN(name, offset) {(name), (offset), false}

static const cxta_field_descriptor cxta_swing_pivots_fields[] = {
    CXTA_FIELD("high", offsetof(cxta_struct_pivot_state, high)),
    CXTA_FIELD("low", offsetof(cxta_struct_pivot_state, low)),
    CXTA_FIELD("is_high", offsetof(cxta_struct_pivot_state, is_high)),
    CXTA_FIELD("is_low", offsetof(cxta_struct_pivot_state, is_low)),
};

static const cxta_field_descriptor cxta_pivot_points_fields[] = {
    CXTA_FIELD("pp", offsetof(cxta_struct_pivot_points_result, pp)),
    CXTA_FIELD("r1", offsetof(cxta_struct_pivot_points_result, r1)),
    CXTA_FIELD("r2", offsetof(cxta_struct_pivot_points_result, r2)),
    CXTA_FIELD("r3", offsetof(cxta_struct_pivot_points_result, r3)),
    CXTA_FIELD("s1", offsetof(cxta_struct_pivot_points_result, s1)),
    CXTA_FIELD("s2", offsetof(cxta_struct_pivot_points_result, s2)),
    CXTA_FIELD("s3", offsetof(cxta_struct_pivot_points_result, s3)),
};

static const cxta_field_descriptor cxta_structure_fields[] = {
    CXTA_FIELD("hh", offsetof(cxta_struct_structure_state, hh)),
    CXTA_FIELD("hl", offsetof(cxta_struct_structure_state, hl)),
    CXTA_FIELD("lh", offsetof(cxta_struct_structure_state, lh)),
    CXTA_FIELD("ll", offsetof(cxta_struct_structure_state, ll)),
    CXTA_FIELD("last_high", offsetof(cxta_struct_structure_state, last_high)),
    CXTA_FIELD("last_low", offsetof(cxta_struct_structure_state, last_low)),
    CXTA_FIELD("strength", offsetof(cxta_struct_structure_state, strength)),
};

static const cxta_field_descriptor cxta_bos_fields[] = {
    CXTA_FIELD("bos_up", offsetof(cxta_struct_bos_state, bos_up)),
    CXTA_FIELD("bos_down", offsetof(cxta_struct_bos_state, bos_down)),
    CXTA_FIELD("level", offsetof(cxta_struct_bos_state, level)),
    CXTA_FIELD("choch_up", offsetof(cxta_struct_bos_state, choch_up)),
    CXTA_FIELD("choch_down", offsetof(cxta_struct_bos_state, choch_down)),
};

static const cxta_field_descriptor cxta_fvg_fields[] = {
    CXTA_FIELD_HIDDEN("bullish", offsetof(cxta_struct_fvg_state, bullish)),
    CXTA_FIELD_HIDDEN("bearish", offsetof(cxta_struct_fvg_state, bearish)),
    CXTA_FIELD("top", offsetof(cxta_struct_fvg_state, top)),
    CXTA_FIELD("bottom", offsetof(cxta_struct_fvg_state, bottom)),
    CXTA_FIELD("filled", offsetof(cxta_struct_fvg_state, filled)),
};

static const cxta_field_descriptor cxta_order_block_fields[] = {
    CXTA_FIELD("bullish", offsetof(cxta_struct_order_block_state, bullish)),
    CXTA_FIELD("bearish", offsetof(cxta_struct_order_block_state, bearish)),
    CXTA_FIELD("top", offsetof(cxta_struct_order_block_state, top)),
    CXTA_FIELD("bottom", offsetof(cxta_struct_order_block_state, bottom)),
    CXTA_FIELD("strength", offsetof(cxta_struct_order_block_state, strength)),
};

static const cxta_field_descriptor cxta_liquidity_fields[] = {
    CXTA_FIELD_HIDDEN("buy_side", offsetof(cxta_struct_liquidity_state, buy_side)),
    CXTA_FIELD_HIDDEN("sell_side", offsetof(cxta_struct_liquidity_state, sell_side)),
    CXTA_FIELD("level", offsetof(cxta_struct_liquidity_state, level)),
    CXTA_FIELD("swept", offsetof(cxta_struct_liquidity_state, swept)),
};

static const cxta_field_descriptor cxta_sfp_fields[] = {
    CXTA_FIELD("sfp_high", offsetof(cxta_struct_sfp_state, sfp_high)),
    CXTA_FIELD("sfp_low", offsetof(cxta_struct_sfp_state, sfp_low)),
    CXTA_FIELD("level", offsetof(cxta_struct_sfp_state, level)),
};

static const cxta_field_descriptor cxta_swing_anchor_vwap_fields[] = {
    CXTA_FIELD("value", offsetof(cxta_struct_vwap_result, value)),
    CXTA_FIELD("direction", offsetof(cxta_struct_vwap_result, direction)),
    CXTA_FIELD("reanchor", offsetof(cxta_struct_vwap_result, reanchor)),
    CXTA_FIELD("anchor_price", offsetof(cxta_struct_vwap_result, anchor_price)),
    CXTA_FIELD("anchor_bars", offsetof(cxta_struct_vwap_result, anchor_bars)),
    CXTA_FIELD("hh", offsetof(cxta_struct_vwap_result, hh)),
    CXTA_FIELD("hl", offsetof(cxta_struct_vwap_result, hl)),
    CXTA_FIELD("lh", offsetof(cxta_struct_vwap_result, lh)),
    CXTA_FIELD("ll", offsetof(cxta_struct_vwap_result, ll)),
    CXTA_FIELD("last_high", offsetof(cxta_struct_vwap_result, last_high)),
    CXTA_FIELD("last_low", offsetof(cxta_struct_vwap_result, last_low)),
    CXTA_FIELD("bull_vwap", offsetof(cxta_struct_vwap_result, bull_vwap)),
    CXTA_FIELD("bear_vwap", offsetof(cxta_struct_vwap_result, bear_vwap)),
};

static const cxta_field_descriptor cxta_wedge_fields[] = {
    CXTA_FIELD("upper", offsetof(cxta_struct_wedge_state, upper)),
    CXTA_FIELD("lower", offsetof(cxta_struct_wedge_state, lower)),
    CXTA_FIELD("width", offsetof(cxta_struct_wedge_state, width)),
    CXTA_FIELD("upperSlope", offsetof(cxta_struct_wedge_state, upper_slope)),
    CXTA_FIELD("lowerSlope", offsetof(cxta_struct_wedge_state, lower_slope)),
    CXTA_FIELD_HIDDEN("active", offsetof(cxta_struct_wedge_state, active)),
    CXTA_FIELD_HIDDEN("rising", offsetof(cxta_struct_wedge_state, rising)),
    CXTA_FIELD_HIDDEN("falling", offsetof(cxta_struct_wedge_state, falling)),
    CXTA_FIELD_HIDDEN("breakdown", offsetof(cxta_struct_wedge_state, breakdown)),
    CXTA_FIELD_HIDDEN("breakout", offsetof(cxta_struct_wedge_state, breakout)),
};

static void cxta_swing_pivots_descriptor_eval(const cxta_series_bar_view* view,
                                             const double* args,
                                             size_t nargs,
                                             void* out) {
    cxta_struct_pivot_state value = {0};
    cxta_struct_pivot_args parsed = {0};

    if (view && cxta_descriptor_parse_pivot_args(view, args, nargs, &parsed)) {
        (void)cxta_struct_pivot_state_compute(view, &parsed, &value);
    }
    cxta_descriptor_copy_struct(out, &value, sizeof(value));
}

static void cxta_structure_descriptor_eval(const cxta_series_bar_view* view,
                                         const double* args,
                                         size_t nargs,
                                         void* out) {
    cxta_struct_pivot_state pivot = {0};
    cxta_struct_structure_state value = {0};
    cxta_struct_pivot_args parsed = {0};

    if (view && cxta_descriptor_parse_pivot_args(view, args, nargs, &parsed)) {
        (void)cxta_struct_pivot_state_compute(view, &parsed, &pivot);
        (void)cxta_struct_structure_state_compute(&pivot, &value);
    }
    cxta_descriptor_copy_struct(out, &value, sizeof(value));
}

static void cxta_bos_descriptor_eval(const cxta_series_bar_view* view,
                                     const double* args,
                                     size_t nargs,
                                     void* out) {
    cxta_struct_pivot_state pivot = {0};
    cxta_struct_bos_state value = {0};
    cxta_struct_pivot_args parsed = {0};

    if (view && cxta_descriptor_parse_pivot_args(view, args, nargs, &parsed)) {
        (void)cxta_struct_pivot_state_compute(view, &parsed, &pivot);
        (void)cxta_struct_bos_state_compute(view, &pivot, &value);
    }
    cxta_descriptor_copy_struct(out, &value, sizeof(value));
}

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

static void cxta_sfp_descriptor_eval(const cxta_series_bar_view* view,
                                       const double* args,
                                       size_t nargs,
                                       void* out) {
    cxta_struct_pivot_state pivot = {0};
    cxta_struct_sfp_state value = {0};
    cxta_struct_pivot_args parsed = {0};

    if (view && cxta_descriptor_parse_pivot_args(view, args, nargs, &parsed)) {
        (void)cxta_struct_pivot_state_compute(view, &parsed, &pivot);
        (void)cxta_struct_sfp_state_compute(view, &pivot, &value);
    }
    cxta_descriptor_copy_struct(out, &value, sizeof(value));
}

static void cxta_swing_anchor_vwap_descriptor_eval(const cxta_series_bar_view* view,
                                                   const double* args,
                                                   size_t nargs,
                                                   void* out) {
    cxta_struct_vwap_result value = {0};
    cxta_struct_vwap_args parsed = {0};

    if (view && cxta_struct_vwap_args_parse(args, nargs, &parsed)) {
        (void)cxta_struct_vwap_compute(view, &parsed, &value);
    }
    cxta_descriptor_copy_struct(out, &value, sizeof(value));
}

static void cxta_swing_anchor_vwap_descriptor_step(const cxta_series_bar_view* view,
                                                   size_t index,
                                                   const double* args,
                                                   size_t nargs,
                                                   void* state,
                                                   void* out) {
    cxta_struct_vwap_result value = {0};
    cxta_struct_vwap_args parsed = {0};

    if (view && state && cxta_struct_vwap_args_parse(args, nargs, &parsed)) {
        (void)cxta_struct_vwap_step(
            view, index, &parsed, (cxta_struct_vwap_state*)state, &value);
    }
    cxta_descriptor_copy_struct(out, &value, sizeof(value));
}

static void cxta_wedge_descriptor_eval(const cxta_series_bar_view* view,
                                       const double* args,
                                       size_t nargs,
                                       void* out) {
    cxta_struct_wedge_state value = {0};
    cxta_struct_wedge_args parsed = {0};
    const size_t current_index =
        (view && view->size > 0u) ? cxta_series_clamp_index(view->size, view->index) : 0u;

    if (view && cxta_struct_wedge_args_parse(args, nargs, current_index, &parsed)) {
        (void)cxta_struct_wedge_state_compute(view, &parsed, &value);
        cxta_struct_wedge_state_mask_geometry(&value);
    }
    cxta_descriptor_copy_struct(out, &value, sizeof(value));
}

static void cxta_pivot_points_descriptor_eval(const cxta_series_bar_view* view,
                                              const double* args,
                                              size_t nargs,
                                              void* out) {
    cxta_struct_pivot_points_result value = {0};
    (void)args;
    (void)nargs;

    if (view) {
        (void)cxta_struct_pivot_points_compute(view, &value);
    }
    cxta_descriptor_copy_struct(out, &value, sizeof(value));
}

static const cxta_param_descriptor cxta_swing_pivots_params[] = {
    {"left"},
    {"right"},
    {"lookback"},
};

static const cxta_param_descriptor cxta_structure_params[] = {
    {"left"},
    {"right"},
    {"lookback"},
};

static const cxta_param_descriptor cxta_bos_params[] = {
    {"left"},
    {"right"},
    {"lookback"},
};

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

static const cxta_param_descriptor cxta_sfp_params[] = {
    {"left"},
    {"right"},
    {"lookback"},
};

static const cxta_param_descriptor cxta_swing_anchor_vwap_params[] = {
    {"swing_period"},
    {"apt"},
    {"use_adapt"},
    {"vol_bias"},
    {"atr_period"},
    {"min_apt"},
    {"max_apt"},
};

/** Default strings match `CXTA_STRUCT_VWAP_DEFAULT_*_STR` in `cxta/structure/vwap.h`. */
static const cxta_expr_arg_descriptor cxta_swing_anchor_vwap_expr_args[] = {
    {"swing_period", CXTA_EXPR_ARG_NUMERIC, CXTA_STRUCT_VWAP_DEFAULT_SWING_PERIOD_STR},
    {"apt", CXTA_EXPR_ARG_NUMERIC, CXTA_STRUCT_VWAP_DEFAULT_APT_STR},
    {"use_adapt", CXTA_EXPR_ARG_NUMERIC, CXTA_STRUCT_VWAP_DEFAULT_USE_ADAPT_STR},
    {"vol_bias", CXTA_EXPR_ARG_NUMERIC, CXTA_STRUCT_VWAP_DEFAULT_VOL_BIAS_STR},
    {"atr_period", CXTA_EXPR_ARG_NUMERIC, CXTA_STRUCT_VWAP_DEFAULT_ATR_PERIOD_STR},
    {"min_apt", CXTA_EXPR_ARG_NUMERIC, CXTA_STRUCT_VWAP_DEFAULT_MIN_APT_STR},
    {"max_apt", CXTA_EXPR_ARG_NUMERIC, CXTA_STRUCT_VWAP_DEFAULT_MAX_APT_STR},
};

static const cxta_param_descriptor cxta_wedge_params[] = {
    {"left"},
    {"right"},
    {"lookback"},
    {"max_width_ratio"},
};

const cxta_bridge_fn_spec cxta_pivot_points_bridge_fn_spec = {
    "pivot_points",
    0u,
    0u,
    NULL,
    0u,
    NULL,
    0u,
    1,
};

const cxta_bridge_fn_spec cxta_swing_pivots_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("swing_pivots", 2u, 3u, cxta_swing_pivots_params, 1);
const cxta_bridge_fn_spec cxta_structure_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("structure", 2u, 3u, cxta_structure_params, 1);
const cxta_bridge_fn_spec cxta_bos_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("bos", 2u, 3u, cxta_bos_params, 1);
const cxta_bridge_fn_spec cxta_fvg_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("fvg", 0u, 1u, cxta_fvg_params, 1);
const cxta_bridge_fn_spec cxta_order_block_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("order_block", 2u, 3u, cxta_order_block_params, 1);
const cxta_bridge_fn_spec cxta_liquidity_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("liquidity", 2u, 4u, cxta_liquidity_params, 1);
const cxta_bridge_fn_spec cxta_sfp_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("sfp", 2u, 3u, cxta_sfp_params, 1);
const cxta_bridge_fn_spec cxta_swing_anchor_vwap_bridge_fn_spec = CXTA_BRIDGE_FN_SPEC_EXPR(
    "swing_anchor_vwap",
    0u,
    7u,
    cxta_swing_anchor_vwap_params,
    cxta_swing_anchor_vwap_expr_args,
    1);
const cxta_bridge_fn_spec cxta_wedge_bridge_fn_spec =
    CXTA_BRIDGE_FN_SPEC("wedge", 2u, 4u, cxta_wedge_params, 1);

const cxta_indicator_descriptor cxta_swing_pivots_descriptor = {
    "swing_pivots",
    2,
    3,
    -1,
    -1,
    0,
    CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_STRUCT,
    sizeof(cxta_struct_pivot_state),
    0u,
    cxta_swing_pivots_fields,
    CXTA_ARRAY_COUNT(cxta_swing_pivots_fields),
    NULL,
    cxta_swing_pivots_descriptor_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_swing_pivots_params,
    CXTA_ARRAY_COUNT(cxta_swing_pivots_params),
};

const cxta_indicator_descriptor cxta_pivot_points_descriptor = {
    "pivot_points",
    0,
    0,
    -1,
    -1,
    0,
    CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_STRUCT,
    sizeof(cxta_struct_pivot_points_result),
    0u,
    cxta_pivot_points_fields,
    CXTA_ARRAY_COUNT(cxta_pivot_points_fields),
    NULL,
    cxta_pivot_points_descriptor_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    0u,
};

const cxta_indicator_descriptor cxta_structure_descriptor = {
    "structure",
    2,
    3,
    -1,
    -1,
    6,
    CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_STRUCT,
    sizeof(cxta_struct_structure_state),
    0u,
    cxta_structure_fields,
    CXTA_ARRAY_COUNT(cxta_structure_fields),
    NULL,
    cxta_structure_descriptor_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_structure_params,
    CXTA_ARRAY_COUNT(cxta_structure_params),
};

const cxta_indicator_descriptor cxta_bos_descriptor = {
    "bos",
    2,
    3,
    -1,
    -1,
    0,
    CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_STRUCT,
    sizeof(cxta_struct_bos_state),
    0u,
    cxta_bos_fields,
    CXTA_ARRAY_COUNT(cxta_bos_fields),
    NULL,
    cxta_bos_descriptor_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_bos_params,
    CXTA_ARRAY_COUNT(cxta_bos_params),
};

const cxta_indicator_descriptor cxta_fvg_descriptor = {
    "fvg",
    0,
    1,
    -1,
    -1,
    0,
    CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_STRUCT,
    sizeof(cxta_struct_fvg_state),
    0u,
    cxta_fvg_fields,
    CXTA_ARRAY_COUNT(cxta_fvg_fields),
    NULL,
    cxta_fvg_descriptor_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_fvg_params,
    CXTA_ARRAY_COUNT(cxta_fvg_params),
};

const cxta_indicator_descriptor cxta_order_block_descriptor = {
    "order_block",
    2,
    3,
    -1,
    -1,
    0,
    CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_STRUCT,
    sizeof(cxta_struct_order_block_state),
    0u,
    cxta_order_block_fields,
    CXTA_ARRAY_COUNT(cxta_order_block_fields),
    NULL,
    cxta_order_block_descriptor_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_order_block_params,
    CXTA_ARRAY_COUNT(cxta_order_block_params),
};

const cxta_indicator_descriptor cxta_liquidity_descriptor = {
    "liquidity",
    2,
    4,
    -1,
    -1,
    0,
    CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_STRUCT,
    sizeof(cxta_struct_liquidity_state),
    0u,
    cxta_liquidity_fields,
    CXTA_ARRAY_COUNT(cxta_liquidity_fields),
    NULL,
    cxta_liquidity_descriptor_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_liquidity_params,
    CXTA_ARRAY_COUNT(cxta_liquidity_params),
};

const cxta_indicator_descriptor cxta_sfp_descriptor = {
    "sfp",
    2,
    3,
    -1,
    -1,
    0,
    CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_STRUCT,
    sizeof(cxta_struct_sfp_state),
    0u,
    cxta_sfp_fields,
    CXTA_ARRAY_COUNT(cxta_sfp_fields),
    NULL,
    cxta_sfp_descriptor_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_sfp_params,
    CXTA_ARRAY_COUNT(cxta_sfp_params),
};

const cxta_indicator_descriptor cxta_swing_anchor_vwap_descriptor = {
    "swing_anchor_vwap",
    0,
    7,
    -1,
    -1,
    0,
    CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_STRUCT,
    sizeof(cxta_struct_vwap_result),
    sizeof(cxta_struct_vwap_state),
    cxta_swing_anchor_vwap_fields,
    CXTA_ARRAY_COUNT(cxta_swing_anchor_vwap_fields),
    NULL,
    cxta_swing_anchor_vwap_descriptor_eval,
    NULL,
    NULL,
    NULL,
    cxta_swing_anchor_vwap_descriptor_step,
    cxta_swing_anchor_vwap_params,
    CXTA_ARRAY_COUNT(cxta_swing_anchor_vwap_params),
};

const cxta_indicator_descriptor cxta_wedge_descriptor = {
    "wedge",
    2,
    4,
    -1,
    -1,
    5,
    CXTA_INDICATOR_SCALAR | CXTA_INDICATOR_STRUCT,
    sizeof(cxta_struct_wedge_state),
    0u,
    cxta_wedge_fields,
    CXTA_ARRAY_COUNT(cxta_wedge_fields),
    NULL,
    cxta_wedge_descriptor_eval,
    NULL,
    NULL,
    NULL,
    NULL,
    cxta_wedge_params,
    CXTA_ARRAY_COUNT(cxta_wedge_params),
};
